#pragma once
#include <cstdint>
#define byte uint8_t
#define word uint16_t
#define sbyte int8_t
#define sword int16_t

class OAM {
public:
  OAM();

  byte oamaddr;
  byte dmaaddr;
  byte *dmamem;
  bool requestdma = false;
  word cycles_left;

  byte Fetch(byte);
  byte Fetch();
  void Write(byte, byte);
  void Write(byte);

  void DMA();

  byte sprites_to_draw[8];
  byte num_sprites;

private:
  byte memory[256];
};
