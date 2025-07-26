#include "CPU.hpp"
#include "debug.hpp"
#include <sstream>
#include <string>
#include <stdexcept>

CPU::CPU() {

}

CPU::~CPU() {

}

void CPU::Init(ROM *r) {
  rom = r;
  
  pc = rom->Fetch(ROM::VECTOR_RESET_ADDR);
}

byte CPU::Fetch(word addr) {
  if (addr >= 0x8000) return rom->Fetch(addr);

  return memory[addr]; // TODO: magic registers
}

byte CPU::FetchWord(word addr) {
  return Fetch(addr) + 256*Fetch(addr+1);
}

byte CPU::FetchPC() {
  byte ret = Fetch(pc++);
  std::stringstream temp;
  std::string temps;
  temp << std::hex << ret;
  temps = temp.str();
  for (auto &c : temps) c = toupper(c);

  debug << temps << " ";
  return ret;
}

void CPU::WaitCycle() {}

void CPU::Write(word addr, byte val) {
  if (addr >= 0x8000) rom->HandleAttemptedWrite(addr);
  
  memory[addr] = val; // TODO: magic registers
}

byte CPU::FetchWordPC() {
  return FetchPC() + 256*FetchPC();
}
