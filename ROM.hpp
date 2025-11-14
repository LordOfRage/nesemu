#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include "INES.hpp"
#define byte uint8_t
#define word uint16_t
#define sbyte int8_t
#define sword int16_t

class ROM {
public:
  ROM(std::string);

  byte Fetch(word addr) { return memory[addr-0x8000]; }
  word FetchWord(word addr) { return Fetch(addr) + 256*Fetch(addr+1); }
  void HandleAttemptedWrite(word addr) { throw std::logic_error("Mapper chips not implemented yet!"); }

  const static word VECTOR_NMI_ADDR = 0xfffa;
  const static word VECTOR_RESET_ADDR = 0xfffc;
  const static word VECTOR_IRQ_ADDR = 0xfffe;

  iNES_header header;
  std::string filename;

private:
  byte *memory;
};
