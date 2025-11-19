#include "CPU.hpp"
#include "ROM.hpp"
#include "PPU.hpp"
#include "debug.hpp"
#include <cstdio>

CPU::CPU(ROM &r, PPU &p) : rom(r), ppu(p) {
  pc = FetchWord(ROM::VECTOR_RESET_ADDR);
  // debug purposes
  processor_flags = 0x24;
  sp = 0xfd;
  accumulator = 0;
  x = 0;
  y = 0;

  memory[5] = 0x99; // resting state: UGH!!!
  for (int i=0; i<0x800; i++) memory[i] = 0;
}

byte CPU::Fetch(word addr) {
  WaitCycle();

  if (addr >= 0x8000) return rom.Fetch(addr);

  if ((0x2000 <= addr && addr <= 0x2007) || addr == PPU::OAMDMA) {
    byte ret = ppu.FetchMMIO(addr);
    // printf("READ PC: %x addr: %x val: %x\n", pc, addr, ret);
    // if (addr != 0x2002) getchar();
    return ret;
  }

  if (addr == 0x722) return 0;

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
  byte ret = Fetch(pc++);
  // debug << to_hex(ret) << " ";
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
  if (addr >= 0x8000) rom.HandleAttemptedWrite(addr);

  else if ((0x2000 <= addr && addr <= 0x2007) || addr == PPU::OAMDMA) {
    ppu.WriteMMIO(addr, val);
    // printf("WRITE PC: %x addr: %x val: %x\n", pc, addr, val);
    // if (addr != 0x2007) getchar();
    // if (pc == 0x8ebe && addr == PPU::PPUDATA) printf("WRITE to ppu\n");
  }
  else if (addr == 0x4016 && val <= 1) {}

  else memory[addr] = val; // TODO: magic registers
}
