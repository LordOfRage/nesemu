#include "PPU.hpp"
#include "debug.hpp"
#include <cstring>
#include <fstream>

PPU::PPU(ROM &r) : rom(r), nmitrigger(false) {
  oam = OAM();
  currscanline = 0;
  currdot = 0;

  std::ifstream file(rom.filename, std::ios::binary); // TODO: figure out why rom.filename reference not working
  file.seekg(16 + 0x4000 * rom.header.v1.prgromunits);
  for (int i=0; i<0x2000 * rom.header.v1.chrromunits; i++) {
    memory[i] = file.get();
  } 

  file.close();
}

// Increments the coarse X value
void PPU::IncX() {
  if ((loopy_v & 0x1f) == 31) {
    loopy_v &= ~0x1f;
    loopy_v ^= 0x0400;
  }
  else loopy_v++;
}

// Increments the fine Y value, and the coarse Y value if overflow happens
void PPU::IncY() {
  if ((loopy_v & 0x7000) != 0x7000) loopy_v += 0x1000;
  else {
    loopy_v &= ~0x7000;
    int y = (loopy_v & 0x03e0) >> 5;
    if (y == 29) {
      y = 0;
      loopy_v ^= 0x0800;
    }
    else if (y == 31) {
      y = 0;
    }
    else y++;

    loopy_v = (loopy_v & ~0x03e0) | (y << 5);
  }
}

// Wraps the coarse X value back to 0
void PPU::WrapX() {
  loopy_v &= ~0x041f;
  loopy_v |= (loopy_t & 0x041f);
}

// Wraps the fine and coarse Y values back to 0
void PPU::WrapY() {
  loopy_v &= ~0x7be0;
  loopy_v |= (loopy_t & 0x7be0);
}

// Performs the fetches that the NES performs for the purpose of drawing, and also other fundemental PPU logic
void PPU::ProcessDot() {
  byte dotstep = currdot % 8; // the current X offset from the 8x8 grid

  if (currscanline == 0 && currdot == 0) {
    sprite0hit_hashappened = false;
    if (ppumask.bgenable || ppumask.spriteenable) loopy_v = loopy_t;
  }

  if (((0 <= currscanline && currscanline <= 239) || currscanline == 261) && ((1 <= currdot && currdot <= 256) || (321 <= currdot && currdot <= 336))) {
    if (dotstep == 1) {
      // Populalate the values of the shift registers using the temporary registers
      palette_lo_register.Populate((bool)(attribute_register_temp & 0x01) * 0xff);
      palette_hi_register.Populate((bool)(attribute_register_temp & 0x02) * 0xff);
      pattern_data_lo_register.Populate(pattern_data_lo_register_temp);
      pattern_data_hi_register.Populate(pattern_data_hi_register_temp);

      address_latch = (loopy_v & 0x0fff) | 0x2000;
    }
    else if (dotstep == 2) {
      // Fetch the tile ID of the next background tile to be drawn
      tileid_register_temp = Fetch(address_latch);
    }
    else if (dotstep == 3) {
      address_latch = 0x23c0 | ((loopy_v & 0x0c00)) | ((loopy_v & 0x001f) >> 2) | ((loopy_v & 0x0380) >> 4);
    }
    else if (dotstep == 4) {
      // Fetch the byte that contains the 2 attribute bits
      attribute_register_temp = Fetch(address_latch);
      // Perform the logic to select the correct 2 bits
      if (loopy_v & 0x02) attribute_register_temp >>= 2;
      if (loopy_v & 0x40) attribute_register_temp >>= 4;
      attribute_register_temp &= 0x03;
    }
    else if (dotstep == 5) {
      address_latch = tileid_register_temp * 16 + (loopy_v >> 12) + 0x1000 * ppuctrl.bgtileselect;
    }
    else if (dotstep == 6) {
      // Get the low bitplane of the current row of the background sprite to be drawn next
      pattern_data_lo_register_temp = Fetch(address_latch);
    }
    else if (dotstep == 7) {
      address_latch = tileid_register_temp * 16 + (loopy_v >> 12) + 8 + 0x1000 * ppuctrl.bgtileselect;
    }
    else if (dotstep == 0) {
      // Get the high bitplane of the current row of the background sprite to be drawn next
      pattern_data_hi_register_temp = Fetch(address_latch);

      // Wrap the coarse X, fine Y and coarse Y if necessary
      if (ppumask.bgenable || ppumask.spriteenable) {
        IncX();
        if (currdot == 256) IncY();
      }
    } //TODO: change to switch case
  }

  // Reset coarse X value
  if (currdot == 257) {
    if (ppumask.bgenable || ppumask.spriteenable) WrapX();
  }

  // Reset coarse Y value
  if (currscanline == 261) {
    if (280 <= currdot && currdot < 305 && (ppumask.bgenable || ppumask.spriteenable)) WrapY();
  }

  // NMI
  if (currscanline == 241 && currdot == 1) {
    ppustatus.vblank = 1;
    if (ppuctrl.nmienable) {
      nmitrigger = true;
    }
  }

  // Reset PPU status flags
  if (currscanline == 261 && currdot == 1) {
    ppustatus.vblank = 0;
    ppustatus.spriteoverflow = 0;
    ppustatus.spritezerohit = 0;
  }

  // Increment the current dot and scanline
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
    // The pixel and palette values of the sprites (behind and in front of the background) and background to be drawn on this pixel
    byte spritepixel_behind = 0;
    byte spritepalette_behind = 0;
    byte bgpixel = 0;
    byte bgpalette = 0;
    byte spritepixel = 0;
    byte spritepalette = 0;
    // Whether sprite 0 hit should be detected or not on this pixel
    bool sprite0hit = false;

    // Sprites
    if (ppumask.spriteenable && !(!ppumask.spriteleftcolumnenable && currdot <= 8)) {
      for (int i=7; i>=0; i--) { // going backwards because lower-indexed sprites have priority over higher-indexed ones
        if (secondary_oam[4*i+3] >= currdot || secondary_oam[4*i+3] < currdot-8) continue;
        if (secondary_oam[4*i] == 0xff) continue;

        byte *to_write_pixel = (secondary_oam[4*i+2] & 0x20) ? &spritepixel_behind : &spritepixel;
        byte *to_write_palette = (secondary_oam[4*i+2] & 0x20) ? &spritepalette_behind : &spritepalette;

        byte pix = spritelo[i].Fetch() + 2*spritehi[i].Fetch();
        *to_write_pixel = pix ? pix : *to_write_pixel;
        *to_write_palette = pix ? secondary_oam[4*i+2] % 4 : *to_write_palette;

        if (i != sprite0ind) continue;
        sprite0hit = pix;
        sprite0hit &= !(currdot == 256);
      }
    }
    // Background
    if (ppumask.bgenable && !(!ppumask.bgleftcolumnenable && currdot <= 8)) {
      bgpixel = pattern_data_hi_register.FetchBit(loopy_x)*2 + pattern_data_lo_register.FetchBit(loopy_x);
      bgpalette = palette_hi_register.FetchBit(loopy_x)*2 + palette_lo_register.FetchBit(loopy_x);
    }
    else {
      // Perform dummy fetches to make the shift registers behave correctly
      pattern_data_hi_register.Fetch();
      pattern_data_lo_register.Fetch();
      palette_lo_register.Fetch();
      palette_hi_register.Fetch();
    }
    word index = currscanline * 256 + currdot - 1;

    // Draw the pixels in order of least to most precedence
    display[index] = memory[0x3f10];
    display[index] = (spritepixel_behind != 0) ? memory[0x3f10 + spritepalette_behind*4 + spritepixel_behind] : display[index];
    display[index] = (bgpixel != 0) ? memory[0x3f00 + bgpalette*4 + bgpixel] : display[index];
    display[index] = (spritepixel != 0) ? memory[0x3f10 + spritepalette*4 + spritepixel] : display[index];

    // Sprite 0 hit logic
    if (sprite0hit && !sprite0hit_hashappened) {
      ppustatus.spritezerohit = true;
      sprite0hit_hashappened = true;
    }
  }

  else if (0 <= currscanline && currscanline <= 239 && 321 <= currdot && currdot <= 336 && ppumask.bgenable) {
    // More dummy fetches to make the shift registers behave correctly
    pattern_data_hi_register.Fetch();
    pattern_data_lo_register.Fetch();
    palette_hi_register.Fetch();
    palette_lo_register.Fetch();
  }
}

void PPU::SpriteEvaluation() {
  if (currdot == 257) { // TODO: spread logic across cycles 65-256
    memset(secondary_oam, 0xff, 32);
    int num_sprites = 0;
    sprite0ind = -1;

    // Check for all the sprites that overlap with the next scanline
    for (int sprite = 0; sprite < 64; sprite++) {
      byte curry = oam.Fetch(4*sprite);
      if (currscanline-7 <= curry && curry < currscanline+1) {
        secondary_oam[4*num_sprites + 0] = oam.Fetch(4*sprite + 0);
        secondary_oam[4*num_sprites + 1] = oam.Fetch(4*sprite + 1);
        secondary_oam[4*num_sprites + 2] = oam.Fetch(4*sprite + 2);
        secondary_oam[4*num_sprites + 3] = oam.Fetch(4*sprite + 3);

        if (sprite == 0) sprite0ind = num_sprites; // Set sprite0ind for later use

        if (++num_sprites == 8) break; // TODO: sprite overflow
      }
    }
  }
  else if (currdot == 340) {
    for (int i=0; i<8; i++) {
      byte spritey = secondary_oam[4*i + 0]; // Y value of sprite
      byte spriteid = secondary_oam[4*i + 1]; // Tile ID of sprite
      byte spriteattr = secondary_oam[4*i + 2]; // Attribute bits of sprite
      byte spritex = secondary_oam[4*i + 3]; // X value of sprite

      byte sy = spritey - currscanline + 7; // Y offset within the tile data to read from

      if (!(spriteattr & 0x80)) sy = 7-sy; // if flipped vertically

      byte memlo = memory[16*spriteid + sy + 0x1000*!ppuctrl.bgtileselect];
      byte memhi = memory[16*spriteid + sy + 8 + 0x1000*!ppuctrl.bgtileselect];

      // Flips the bits in a byte e.g. 0b11001010 becomes 0b01010011
      auto flip = [](byte *in) {
        const byte FLIPPED_NYBBLES[] = {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
        *in = FLIPPED_NYBBLES[*in >> 4] + 16*FLIPPED_NYBBLES[*in % 16];
      };

      if (spriteattr & 0x40) { // if flipped horizontally
        flip(&memlo);
        flip(&memhi);
      }

      spritelo[i].Populate(memlo);
      spritehi[i].Populate(memhi);
    }
  }
}

void PPU::PerformCycles(byte cycles) {
  for (int i=0; i<cycles; i++) {
    SpriteEvaluation();
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
    case PPUSTATUS: {
      loopy_w = 0; // reset the shared read/write latch
      byte ret = ppustatus.vblank*128 + ppustatus.spritezerohit*64 + ppustatus.spriteoverflow*32;
      // reset the PPUSTATUS bits 
      if (ppustatus.vblank) ppustatus.vblank = false;
      if (ppustatus.spritezerohit) ppustatus.spritezerohit = false;
      if (ppustatus.spriteoverflow) ppustatus.spriteoverflow = false;
      return ret;
    }
    case OAMDATA:
      return oam.Fetch();
    case PPUDATA: {
      byte ret = ppudata_readbuff;
      ppudata_readbuff = memory[loopy_v];
      loopy_v += ppuctrl.incrementmode ? 32 : 1; // auto-increment the loopy_v register depending on the increment mode
      return ret;
    }
  }

  return 0; // TODO: implement ub for reading write-only registers?
}

void PPU::WriteMMIO(word addr, byte val) {
  switch (addr) {
    case PPUCTRL:
      // set the internal state using the bits from the input
      ppuctrl.nmienable = (bool)(val & 0x80);
      ppuctrl.ppumasterslave = (bool)(val & 0x40);
      ppuctrl.spriteheight = (bool)(val & 0x20);
      ppuctrl.bgtileselect = (bool)(val & 0x10);
      ppuctrl.spritetileselect = (bool)(val & 0x08);
      ppuctrl.incrementmode = (bool)(val & 0x04);
      ppuctrl.nametableselect = (val & 0x03);

      // bit logic to set the nametable X and Y bits in the t register
      val &= 3;
      loopy_t &= ~0x0c00;
      loopy_t |= ((word)val << 10);
      break;

    case PPUMASK:
      // set the internal state using the bits from the input
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
      // set the internal state using the bits from the input
      ppustatus.vblank = (bool)(val & 0x80);
      ppustatus.spritezerohit = (bool)(val & 0x40);
      ppustatus.spriteoverflow = (bool)(val & 0x20);
      break;
    
    case OAMADDR:
      oam.oamaddr = val;
      break;

    case OAMDATA:
      oam.Write(val);
      break;
 
    case PPUSCROLL:
      // bitwise magic
      if (!loopy_w) {
        loopy_t &= ~0x1f;
        loopy_t |= (val & 0xf8) >> 3;
        loopy_x = val & 7;
      }
      else {
        word bitmask = 0;
        bitmask |= ((val & ~3) << 2);
        bitmask |= (((word)val & 3) << 12);

        loopy_t &= ~0b111001111100000;
        loopy_t |= bitmask;
      }
      loopy_w = !loopy_w; // toggle read/write latch
      break;

    case PPUADDR:
      // transfer the address to fetch from to t one byte at a time
      if (!loopy_w) {
        loopy_t &= ~0xff00;
        loopy_t |= ((val & 0x3f) << 8);
      }
      else {
        loopy_t &= ~0xff;
        loopy_t |= val;
        loopy_v = loopy_t; //TODO: delay this by 1 to 1.5 dots maybe?
      }
      loopy_w = !loopy_w; // toggle read/write latch
      break;

    case PPUDATA:
      if (!IsRendering()) {
        word add = loopy_v & ~0xc000; // keep the address within the range [0-0x4000]
        loopy_v += (ppuctrl.incrementmode) ? 32 : 1; // auto-increment loopy_v register

        // mirroring logic
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
        else if (0x0 <= add && add <= 0x1fff) return; // prevent writes to this memory from happening
        memory[add] = val;
      } // MAYBE TODO: implement scrolling during rendering
      break;

    case OAMDMA:
      oam.dmaaddr = val;
      oam.requestdma = true;
      break;
  }
}

