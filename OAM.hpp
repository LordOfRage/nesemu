#pragma once
#include <cstdint>
#define byte uint8_t
#define word uint16_t
#define sbyte int8_t
#define sword int16_t

class OAM {
public:
  OAM();

  byte oamaddr; // current address within OAM data
  byte dmaaddr; // current address within CPU memory page to perform DMA on
  byte *dmamem; // address in host machine where the DMA memory starts
  bool requestdma = false;
  word cycles_left; // cycles left to perform DMA

  byte Fetch(byte);
  byte Fetch(); // implicit fetch using oamaddr as address
  void Write(byte, byte);
  void Write(byte); // implicit write using oamaddr as address

  void DMA();

  byte sprites_to_draw[8]; // sprites in the next scanline, up to a maximum of 8
  byte num_sprites; // number of sprites in sprites_to_draw

private:
  byte memory[256];
};
