#include "CPU.hpp"
#include <stdexcept>

CPU::CPU() {

}

CPU::~CPU() {

}

byte CPU::Fetch(word addr) {
  if (addr >= 0x8000) throw std::logic_error("ROM not implemented yet");

  return memory[addr]; // TODO: magic registers
}

byte CPU::FetchWord(word addr) {
  return Fetch(addr) + 256*Fetch(addr+1);
}

byte CPU::FetchPC() {
  return Fetch(pc++);
}

byte CPU::FetchWordPC() {
  return FetchPC() + 256*FetchPC();
}
