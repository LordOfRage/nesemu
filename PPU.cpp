#include "PPU.hpp"
#include "debug.hpp"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>

PPU::PPU(ROM &r) : rom(r), nmitrigger(false) {
  currscanline = 0;
  currdot = 0;

  std::ifstream file(rom.filename, std::ios::binary); // TODO: figure out why rom.filename reference not working
  file.seekg(16 + 0x4000 * rom.header.v1.prgromunits);
  for (int i=0; i<0x2000 * rom.header.v1.chrromunits; i++) {
    memory[i] = file.get();
  } 

  file.close();
}

void PPU::IncX() {
  if ((v & 0x1f) == 31) {
    v &= ~0x1f;
    v ^= 0x0400;
  }
  else v++;
}

void PPU::IncY() {
  if ((v & 0x7000) != 0x7000) v += 0x1000;
  else {
    v &= ~0x7000;
    byte y = (v & 0x03e0) >> 5;
    if (y == 29) {
      y = 0;
      v ^= 0x0800;
    }
    else if (y == 31) {
      y = 0;
    }
    else y++;

    v = (v & ~0x03e0) | (y << 5);
  }
}

void PPU::WrapX() {
  v &= ~0x041f;
  v |= (t & 0x041f);
}

void PPU::WrapY() {
  v &= ~0x7be0;
  v |= (t & 0x7be0);
}

void PPU::ProcessDot() {
  if (currscanline == 30 && currdot == 92 && ppumask.spriteenable) ppustatus.spritezerohit = 1; //hack until sprites implemented

  if ((ppumask.bgenable || ppumask.spriteenable) && currscanline == 0 && currdot == 0) {
    v = t;
  }
  if (((0 <= currscanline && currscanline <= 239) || currscanline == 261) && ((1 <= currdot && currdot <= 256) || (321 <= currdot && currdot <= 336))) {
    // x = ++x % 8;

    byte dotstep = currdot % 8;

    if (dotstep == 1) {
      // printf("dot for sr update %d\n", currdot);
      pallete_lo_register.Populate((bool)(attribute_register_temp & 1) * 0xff);
      pallete_hi_register.Populate((bool)(attribute_register_temp & 2) * 0xff);
      pattern_data_lo_register.Populate(pattern_data_lo_register_temp);
      pattern_data_hi_register.Populate(pattern_data_hi_register_temp);
      address_latch = (v & 0x0fff) | 0x2000;
    }
    else if (dotstep == 2) {
      tileid_register_temp = Fetch(address_latch);
    }
    else if (dotstep == 3) {
      address_latch = 0x23c0 | ((v & 0x0c00)) | ((v & 0x001f) >> 2) | ((v & 0x0380) >> 4);
    }
    else if (dotstep == 4) {
      attribute_register_temp = Fetch(address_latch);
      if (v & 0x02) attribute_register_temp >>= 2;
      if (v & 0x40) attribute_register_temp >>= 4;
    }
    else if (dotstep == 5) {
      address_latch = tileid_register_temp * 16 + (v >> 12) + 0x1000 * ppuctrl.bgtileselect;
    }
    else if (dotstep == 6) {
      pattern_data_lo_register_temp = Fetch(address_latch);
    }
    else if (dotstep == 7) {
      address_latch = tileid_register_temp * 16 + (v >> 12) + 8 + 0x1000 * ppuctrl.bgtileselect;
    }
    else if (dotstep == 0) {
      pattern_data_hi_register_temp = Fetch(address_latch);
      if (ppumask.bgenable || ppumask.spriteenable) {
        IncX();
        if (currdot == 256) IncY();
      }
    } //TODO: change to switch case
  }
  if (currdot == 257) {
    if (ppumask.bgenable || ppumask.spriteenable) WrapX();
  }
  if (currscanline == 261) {
    if (280 <= currdot && currdot < 305 && (ppumask.bgenable || ppumask.spriteenable)) WrapY();
  }
  if (currscanline == 241 && currdot == 1) {
    ppustatus.vblank = 1;
    if (ppuctrl.nmienable) {
      nmitrigger = true;
    }
  }
  if (currscanline == 261 && currdot == 1) {
    ppustatus.vblank = 0;
    ppustatus.spriteoverflow = 0;
    ppustatus.spritezerohit = 0;
  }

  currdot++;
  if (currdot == 341) {
    currdot = 0;
    currscanline++;
    if (currscanline == 262) {
      currscanline = 0;
    }
  }
}

void PPU::DrawDot() {
  if (0 <= currscanline && currscanline <= 239 && 1 <= currdot && currdot <= 256) {
    if (ppumask.bgenable) {
      byte pixel = pattern_data_hi_register.FetchBit(x)*2 + pattern_data_lo_register.FetchBit(x);
      byte pallete = pallete_hi_register.FetchBit(x)*2 + pallete_lo_register.FetchBit(x);

      word index = currscanline * 256 + currdot - 1;

      display[index] = (pixel != 0) ? memory[0x3f00 + pallete*4 + pixel] : memory[0x3f10];
    }
  }

  else if (0 <= currscanline && currscanline <= 239 && 321 <= currdot && currdot <= 336 && ppumask.bgenable) {
    pattern_data_hi_register.Fetch();
    pattern_data_lo_register.Fetch();
    pallete_hi_register.Fetch();
    pallete_lo_register.Fetch();
  }

  if (currscanline == 241 && currdot == 1) {
    for (int i=0x23c0; i<0x2400; i+=16) {
      for (int j=0; j<16; j++) {
        // printf("%x ", memory[i+j]);
      }
      // printf("\n");
    }
  }
}

void PPU::PerformCycles(byte cycles) {
  for (int i=0; i<cycles; i++) {
    ProcessDot();
    DrawDot();
  }
}

byte PPU::Fetch(word addr) {
  return memory[addr];
}

void PPU::Write(word addr, byte val) {
  if (0x0 <= addr && addr <= 0x1fff) return;
  memory[addr] = val;
}

byte PPU::FetchMMIO(word addr) {
  switch (addr) {
    byte ret;
    case PPUSTATUS:
      w = 0;
      ret = ppustatus.vblank*128 + ppustatus.spritezerohit*64 + ppustatus.spriteoverflow*32;
      if (ppustatus.vblank) ppustatus.vblank = false;
      if (ppustatus.spritezerohit) ppustatus.spritezerohit = false;
      if (ppustatus.spriteoverflow) ppustatus.spriteoverflow = false;
      return ret;
    case PPUDATA:
      // printf("\x1b[31mread from v=%x: %x scanline %d dot %d\x1b[0m\n", v, ppudata_readbuff, currscanline, currdot);
      ret = ppudata_readbuff;
      ppudata_readbuff = memory[v];
      v += ppuctrl.incrementmode ? 32 : 1;
      return ret;
  }

  return 0; // TODO: implement ub for reading write-only registers?
}

void PPU::WriteMMIO(word addr, byte val) {
  switch (addr) {
    case PPUCTRL:
      ppuctrl.nmienable = (bool)(val & 0x80);
      ppuctrl.ppumasterslave = (bool)(val & 0x40);
      ppuctrl.spriteheight = (bool)(val & 0x20);
      ppuctrl.bgtileselect = (bool)(val & 0x10);
      ppuctrl.spritetileselect = (bool)(val & 0x08);
      ppuctrl.incrementmode = (bool)(val & 0x04);
      ppuctrl.nametableselect = (val & 0x03);
      val &= 3;
      t &= ~0x0c00;
      t |= ((word)val << 10);
      break;

    case PPUMASK:
      ppumask.colouremphasisblue = (bool)(val & 0x80);
      ppumask.colouremphasisgreen = (bool)(val & 0x40);
      ppumask.colouremphasisred = (bool)(val & 0x20);
      ppumask.spriteenable = (bool)(val & 0x10);
      ppumask.bgenable = (bool)(val & 0x08);
      ppumask.spriteleftcolumnenable = (bool)(val & 0x04);
      ppumask.bgleftcolumnenable = (bool)(val & 0x02);
      ppumask.greyscale = (bool)(val & 0x01);
      break;

    case PPUSTATUS:
      ppustatus.vblank = (bool)(val & 0x80);
      ppustatus.spritezerohit = (bool)(val & 0x40);
      ppustatus.spriteoverflow = (bool)(val & 0x20);
    
    // TODO: OAM MMIO registers
 
    case PPUSCROLL:
      if (!w) {
        t &= ~0x1f;
        t |= (val & 0xf8) >> 3;
        x = val & 7;
      }
      else {
        word bitmask = 0;
        bitmask |= ((val & ~3) << 2);
        bitmask |= (((word)val & 3) << 12);

        t &= ~0b111001111100000;
        t |= bitmask;
      }
      w = !w;
      break;

    case PPUADDR:
      if (!w) {
        t &= ~0xff00;
        t |= ((val & 0x3f) << 8);
      }
      else {
        t &= ~0xff;
        t |= val;
        v = t; //TODO: delay this by 1 to 1.5 dots maybe?
        // printf("\x1b[33mv is now %x\x1b[0m\n", v);
      }
      w = !w;
      break;

    case PPUDATA:
      if (!IsRendering()) {
        word add = v & ~0xc000;
        v += (ppuctrl.incrementmode) ? 32 : 1;

        // if (0x23c0 <= add && add < 0x2400) printf("Write to %x: %x\n", add, val);
        // else printf("ppu addr %x\n", add);
        
        if (rom.header.v1.is_vertical_mirror) {
          if (0x2000 <= add && add < 0x2800) {
            memory[add + 0x800] = val;
          }
          else if (0x2800 <= add && add < 0x3000) {
            memory[add - 0x800] = val;
          }
        }
        else {
          if (0x2000 <= add && add < 0x2400) {
            memory[add + 0x400] = val;
          }
          else if (0x2400 <= add && add < 0x2800) {
            memory[add - 0x400] = val;
          }
          else if (0x2800 <= add && add < 0x2c00) {
            memory[add + 0x400] = val;
          }
          else if (0x2c00 <= add && add < 0x3000) {
            memory[add - 0x400] = val;
          }
        }

        if (0x3f00 <= add && add <= 0x3f1f) {
          memory[(add) + 0x20] = val;
          memory[(add) + 0x40] = val;
          memory[(add) + 0x60] = val;
          memory[(add) + 0x80] = val;
          memory[(add) + 0xa0] = val;
          memory[(add) + 0xc0] = val;
          memory[(add) + 0xe0] = val;
        }
        else if (0x0 <= add && add <= 0x1fff) return;
        memory[add] = val;
      } // MAYBE TODO: implement scrolling during rendering
      break;

  }
}

