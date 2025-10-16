#include "CPU.hpp"
#include "ROM.hpp"

CPU::~CPU() {

}

CPU::CPU(ROM &r) : rom(r) {
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
  if (addr >= 0x8000) return rom.Fetch(addr);

  return memory[addr]; // TODO: magic registers
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

void CPU::WaitCycle() {}

void CPU::Write(word addr, byte val) {
  if (addr >= 0x8000) rom.HandleAttemptedWrite(addr);

  else memory[addr] = val; // TODO: magic registers
}
