#pragma once
#include <cstdint>
#define byte uint8_t
#define word uint16_t

struct ShiftReg {
  word val;

  void Populate(byte val) { this->val &= 0xff00; this->val |= val; }

  bool Fetch() { bool ret = val & 0x80; val <<= 1; return ret; }
  bool FetchBit(byte bit) { bool ret = val & (0x4000 >> bit); val <<= 1; return ret; }
};
