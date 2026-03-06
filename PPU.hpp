/*
 *
 * A WORD OF WARNING!
 *
 * The PPU is a complicated beast. Even with all the comments in the world, it is almost impossible to explain
 * what this does to another programmer unless they already know about the PPU.
 *
 * I recommend consulting the NesDev wiki articles if there is something that doesn't make sense.
 *
 * YOU HAVE BEEN WARNED!
 *
 */

#pragma once
#include "ROM.hpp"
#include "OAM.hpp"
#include "ShiftReg16.hpp"
#include "ShiftReg8.hpp"
#include <array>
#include <cstdint>

class PPU {
public:
  PPU(ROM&);

  static const int WIDTH_PIXELS = 256;
  static const int HEIGHT_PIXELS = 240;
  static const int AREA_PIXELS = WIDTH_PIXELS*HEIGHT_PIXELS;

  // Handling MMIO reads/writes done by the CPU
  byte FetchMMIO(word);
  void WriteMMIO(word, byte);

  byte Fetch(word);
  void Write(word, byte);

  void PerformCycles(byte);
  void ProcessDot();
  void DrawDot();
  void SpriteEvaluation();

  std::array<uint8_t, AREA_PIXELS> display;
  bool nmitrigger;

  // PPU MMIO register addresses
  static const word PPUCTRL = 0x2000;
  static const word PPUMASK = 0x2001;
  static const word PPUSTATUS = 0x2002;
  static const word OAMADDR = 0x2003;
  static const word OAMDATA = 0x2004;
  static const word PPUSCROLL = 0x2005;
  static const word PPUADDR = 0x2006;
  static const word PPUDATA = 0x2007;
  static const word OAMDMA = 0x4014;

  OAM oam;

private:
  std::array<uint8_t, 0x4000> memory;

  // when PPU data is read from by the CPU using PPUDATA, there is a one-read delay between asking for the value and getitng it back, due to this read buffer that is used
  byte ppudata_readbuff = 0;

  struct {
    unsigned nmienable: 1;
    unsigned ppumasterslave: 1; // yes, this term is still used
    unsigned spriteheight: 1;
    unsigned bgtileselect: 1;
    unsigned spritetileselect: 1;
    unsigned incrementmode: 1;
    unsigned nametableselect: 2;
  } ppuctrl;

  struct {
    unsigned colouremphasisblue: 1;
    unsigned colouremphasisgreen: 1;
    unsigned colouremphasisred: 1;
    unsigned spriteenable: 1;
    unsigned bgenable: 1;
    unsigned spriteleftcolumnenable: 1;
    unsigned bgleftcolumnenable: 1;
    unsigned greyscale: 1;
  } ppumask;

  struct {
    unsigned vblank: 1;
    unsigned spritezerohit: 1;
    unsigned spriteoverflow: 1;
  } ppustatus;

  // PPU registers which were documented by someone called Loopy. Read this for more on these registers:
  // www.nesdev.org/wiki/PPU_scrolling#PPU_internal_registers
  word loopy_v, loopy_t;
  byte loopy_x;
  bool loopy_w;

  // current scanline and dot within scanline being drawn respectively
  // can be larger than 240/256 respectively during v-blank/h-blank respectively
  // currscanline ranges between 0 and 261, currdot ranges between 0 and 341
  word currscanline, currdot;

  // temporary registers to hold certain data until the appropriate shift registers can be safely filled
  byte tileid_register_temp, attribute_register_temp, pattern_data_lo_register_temp, pattern_data_hi_register_temp;
  // shift registers to hold certain data about the next 16 pixels to be drawn
  // read this for more: https://www.nesdev.org/wiki/PPU_rendering
  ShiftReg16 palette_lo_register, palette_hi_register, pattern_data_lo_register, pattern_data_hi_register;

  // the low and high bitplanes for the current row of the 8 sprites to be drawn on the next scanline
  ShiftReg8 spritelo[8];
  ShiftReg8 spritehi[8];

  // temporary register to store the address to fetch from before performing the fetch on the next cycle
  // only used when appropriate rather than every fetch for the sake of simplicity
  word address_latch;

  byte secondary_oam[32];
  int sprite0ind; // index into secondary_oam where sprite 0 is found; set to -1 if not present
  bool sprite0hit_hashappened; // to prevent sprite 0 hit activating more than once in one frame

  void IncX();
  void IncY();
  void WrapX();
  void WrapY();

  inline bool IsRendering() { return ((ppumask.spriteenable || ppumask.bgenable) && (0 <= currscanline && currscanline <= 239 || currscanline == 261) && (0 <= currdot && currdot <= 255)); };

  ROM const &rom;
};
