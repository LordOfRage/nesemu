#include "CPU.hpp"
#include "ROM.hpp"
#include "PPU.hpp"
#include "controller.hpp"

CPU::CPU(ROM &r, PPU &p, Controller &c) : rom(r), ppu(p), controller(c) {
  pc = FetchWord(ROM::VECTOR_RESET_ADDR);
}

byte CPU::Fetch(word addr) {
  WaitCycle();

  if (addr >= 0x8000) return rom.Fetch(addr);

  if ((0x2000 <= addr && addr <= 0x2007) || addr == PPU::OAMDMA) {
    return ppu.FetchMMIO(addr);
  }

  if (addr == 0x4016) return controller.FetchMMIO(addr);

  return memory[addr];
}

byte CPU::Fetch(word addr, byte index) {
  byte lo = addr + index;
  byte hi = addr >> 8;
  return Fetch(lo + 256*hi);
}

word CPU::FetchWord(word addr) {
  return Fetch(addr) + 256*Fetch(addr+1);
}

word CPU::FetchWord(word addr, byte index) {
  byte lo = addr + index;
  byte hi = addr >> 8;
  return Fetch(lo + 256*hi) + 256*Fetch((hi)*256 + ((lo+1)%256));
}

byte CPU::FetchPC() {
  byte ret = Fetch(pc++);
  return ret;
}

byte CPU::FetchPC(byte index) {
  return Fetch(pc++, index);
}

word CPU::FetchWordPC() {
  return FetchPC() + 256*FetchPC();
}

void CPU::WaitCycle() {
  cycles_passed++;
}

void CPU::TriggerNMI() {
  Push(pc >> 8);
  Push(pc & 0xff);
  Push(processor_flags);
  pc = FetchWord(ROM::VECTOR_NMI_ADDR);
}

void CPU::Write(word addr, byte val) {
  WaitCycle();
  if (addr >= 0x8000) rom.HandleAttemptedWrite(addr);

  else if ((0x2000 <= addr && addr <= 0x2007) || addr == PPU::OAMDMA) {
    ppu.WriteMMIO(addr, val);
  }

  else if (addr == 0x4016) controller.WriteMMIO(addr, val);

  else memory[addr] = val;
}

void CPU::CheckDMA() {
  if (!ppu.oam.requestdma) return;

  ppu.oam.requestdma = false;
  ppu.oam.dmamem = memory.data() + ppu.oam.dmaaddr*256;
  ppu.oam.cycles_left = 512;
}

void CPU::TickDMA() {
  for (int i=0; i<cycles_passed; i++) ppu.oam.DMA();
}
