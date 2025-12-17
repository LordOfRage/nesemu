#pragma once
#include <cstdint>
#define byte uint8_t
#define word uint16_t

struct ShiftReg8 {
  byte val;

  void Populate(byte val) { this->val = val; }

  bool Fetch() { bool ret = val & 0x80; val <<= 1; return ret; }
  bool FetchBit(byte bit) { bool ret = val & (0x40 >> bit); val <<= 1; return ret; }
};
