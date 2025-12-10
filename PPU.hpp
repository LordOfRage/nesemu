#pragma once
#include "ROM.hpp"
#include "OAM.hpp"
#include "ShiftReg.hpp"
#include <array>
#include <cstdint>
#include <vector>

class PPU {
public:
  PPU(ROM&);

  byte FetchMMIO(word);
  void WriteMMIO(word, byte);

  byte Fetch(word);
  void Write(word, byte);

  void PerformCycles(byte);
  void ProcessDot();
  void DrawDot();

  std::vector<uint8_t> pattern_table_left;
  std::vector<uint8_t> pattern_table_right;

  std::array<uint8_t, 256*240> display;
  bool nmitrigger;

  static const word PPUCTRL = 0x2000;
  static const word PPUMASK = 0x2001;
  static const word PPUSTATUS = 0x2002;
  static const word OAMADDR = 0x2003;
  static const word OAMDATA = 0x2004;
  static const word PPUSCROLL = 0x2005;
  static const word PPUADDR = 0x2006;
  static const word PPUDATA = 0x2007;
  static const word OAMDMA = 0x4014;

  std::array<uint8_t, 0x4000> memory;
private:

  byte ppudata_readbuff = 0;

  struct {
    unsigned nmienable: 1;
    unsigned ppumasterslave: 1;
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

  word v, t;
  byte x;
  bool w;

  word currscanline, currdot;

  byte tileid_register_temp, attribute_register_temp, pattern_data_lo_register_temp, pattern_data_hi_register_temp;
  ShiftReg pallete_lo_register, pallete_hi_register, pattern_data_lo_register, pattern_data_hi_register;
  word address_latch;

  void IncX();
  void IncY();
  void WrapX();
  void WrapY();

  inline bool IsRendering() { return ((ppumask.spriteenable || ppumask.bgenable) && (0 <= currscanline && currscanline <= 239 || currscanline == 261) && (0 <= currdot && currdot <= 255)); };

  ROM const &rom;
  OAM const &oam;
};
