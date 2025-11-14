#include "CPU.hpp"
#include "ROM.hpp"
#include "PPU.hpp"

CPU::CPU(ROM &r, PPU &p) : rom(r), ppu(p) {
  pc = FetchWord(ROM::VECTOR_RESET_ADDR);
  // debug purposes
  processor_flags = 0x24;
  sp = 0xfd;
  accumulator = 0;
  x = 0;
  y = 0;

  memory[5] = 0x99; // resting state: UGH!!!
}

byte CPU::Fetch(word addr) {
  WaitCycle();

  if (addr >= 0x8000) return rom.Fetch(addr);

  if ((0x2000 <= addr && addr <= 0x2007) || addr == PPU::OAMDMA) {
    ppu.FetchMMIO(addr);
  }

  return memory[addr];
}

byte CPU::Fetch(word addr, byte index) {
  byte lo = addr + index;
  byte hi = addr >> 8;
  return Fetch(lo + 256*hi);
}

word CPU::FetchWord(word addr, byte index) {
  byte lo = addr + index;
  byte hi = addr >> 8;
  return Fetch(lo + 256*hi) + 256*Fetch((hi)*256 + ((lo+1)%256));
}

byte CPU::FetchPC() {
  return Fetch(pc++);
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
  pc = Fetch(ROM::VECTOR_NMI_ADDR);
}

void CPU::Write(word addr, byte val) {
  if (addr >= 0x8000) rom.HandleAttemptedWrite(addr);

  if ((0x2000 <= addr && addr <= 0x2007) || addr == PPU::OAMDMA) {
    ppu.WriteMMIO(addr, val);
  }

  else memory[addr] = val; // TODO: magic registers
}
