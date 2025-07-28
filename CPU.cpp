#include "CPU.hpp"
#include "debug.hpp"
#include <sstream>
#include <string>
#include <stdexcept>
#include <iostream>

CPU::CPU() {

}

CPU::~CPU() {

}

void CPU::Init(ROM *r) {
  rom = r;
  pc = FetchWord(ROM::VECTOR_RESET_ADDR);
  // debug purposes
  processor_flags = 0x24;
  sp = 0xfd;
  accumulator = 0;
  x = 0;
  y = 0;
}

byte CPU::Fetch(word addr) {
  if (addr >= 0x8000) return rom->Fetch(addr);

  return memory[addr]; // TODO: magic registers
}

word CPU::FetchWord(word addr) {
  return Fetch(addr) + 256*Fetch(addr+1);
}

byte CPU::FetchPC() {
  byte ret = Fetch(pc++);

  debug << to_hex<byte>(ret) << " ";
  return ret;
}

word CPU::FetchWordPC() {
  return FetchPC() + 256*FetchPC();
}

void CPU::WaitCycle() {}

void CPU::Write(word addr, byte val) {
  if (addr >= 0x8000) rom->HandleAttemptedWrite(addr);
  
  memory[addr] = val; // TODO: magic registers
}
