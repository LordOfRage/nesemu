#pragma once
#include <cstdint>
#define byte uint8_t
#define word uint16_t
#define sbyte int8_t
#define sword int16_t

class OAM {
public:
  byte oamaddr;

  byte Fetch(byte);
  void Write(byte, byte);

private:
  byte memory[256];
};
