#pragma once
#include <cstdint>
#define byte uint8_t

struct ShiftReg {
  byte val;

  void Populate(byte val) { this->val = val; }

  bool Fetch() { bool ret = val & 0x80; val <<= 1; return ret; }
};
