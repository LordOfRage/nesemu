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
    int y = (v & 0x03e0) >> 5;
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
  // if (currscanline == 30 && currdot == 92 && ppumask.spriteenable) ppustatus.spritezerohit = 1; //hack until sprites implemented

  if (currscanline == 0 && currdot == 0) {
    sprite0hit_hashappened = false;
    if (ppumask.bgenable || ppumask.spriteenable) v = t;
  }
  if (((0 <= currscanline && currscanline <= 239) || currscanline == 261) && ((1 <= currdot && currdot <= 256) || (321 <= currdot && currdot <= 336))) {
    // x = ++x % 8;

    byte dotstep = currdot % 8;

    if (dotstep == 1) {
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
    // hack to see if oam works
    // for (int i=0; i<256; i+=16) {
    //   for (int j=0; j<16; j++) {
    //     debug << to_hex(oam.Fetch(i+j)) << " ";
    //   }
    //   debug << "\n";
    // }
    // debug << "\n\n";
    // for (int sprite = 0; sprite<64; sprite++) {
    //   byte spritey = oam.Fetch(4*sprite + 0);
    //   byte spriteid = oam.Fetch(4*sprite + 1);
    //   byte spriteattr = oam.Fetch(4*sprite + 2);
    //   byte spritex = oam.Fetch(4*sprite + 3);
    //
    //   if (spritey > 232) continue;
    //   byte pallete = spriteattr % 4;
    //   bool flipx = spriteattr & 0x40;
    //   bool flipy = spriteattr & 0x80;
    //
    //   for (int sx=0; sx<8; sx++) {
    //     for (int sy=0; sy<8; sy++) {
    //       int ax = flipx ? 7-sx : sx;
    //       int ay = flipy ? 7-sy : sy;
    //       bool pixlo = Fetch(spriteid * 16 + ay + 0x1000*!ppuctrl.bgtileselect) & (0x80 >> ax);
    //       bool pixhi = Fetch(spriteid * 16 + ay + 8 + 0x1000*!ppuctrl.bgtileselect) & (0x80 >> ax);
    //
    //       byte pixel = pixlo + 2*pixhi;
    //       if (!pixel) continue;
    //
    //       display[(sy+spritey) * 256 + sx+spritex] = memory[0x3f10 + pallete*4 + pixel];
    //     }
    //   }
    // }

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
    byte spritepixel_behind = 0;
    byte spritepallete_behind = 0;
    byte bgpixel = 0;
    byte bgpallete = 0;
    byte spritepixel = 0;
    byte spritepallete = 0;
    bool sprite0hit = false;

    if (ppumask.spriteenable && !(!ppumask.spriteleftcolumnenable && currdot <= 8)) {
      for (int i=7; i>=0; i--) {
        if (secondary_oam[4*i+3] >= currdot || secondary_oam[4*i+3] < currdot-8) continue;
        if (secondary_oam[4*i] == 0xff) continue;

        byte *to_write_pixel = (secondary_oam[4*i+2] & 0x20) ? &spritepixel_behind : &spritepixel;
        byte *to_write_pallete = (secondary_oam[4*i+2] & 0x20) ? &spritepallete_behind : &spritepallete;

        byte pix = spritelo[i].Fetch() + 2*spritehi[i].Fetch();
        *to_write_pixel = pix ? pix : *to_write_pixel;
        *to_write_pallete = pix ? secondary_oam[4*i+2] % 4 : *to_write_pallete;

        if (i != sprite0ind) continue;
        sprite0hit = pix;
        sprite0hit &= !(currdot == 256);
      }
    }

    if (ppumask.bgenable && !(!ppumask.bgleftcolumnenable && currdot <= 8)) {
      bgpixel = pattern_data_hi_register.FetchBit(x)*2 + pattern_data_lo_register.FetchBit(x);
      bgpallete = pallete_hi_register.FetchBit(x)*2 + pallete_lo_register.FetchBit(x);
    }
    else {
      pattern_data_hi_register.Fetch();
      pattern_data_lo_register.Fetch();
      pallete_lo_register.Fetch();
      pallete_hi_register.Fetch();
    }
    word index = currscanline * 256 + currdot - 1;

    display[index] = memory[0x3f10];
    display[index] = (spritepixel_behind != 0) ? memory[0x3f10 + spritepallete_behind*4 + spritepixel_behind] : display[index];
    display[index] = (bgpixel != 0) ? memory[0x3f00 + bgpallete*4 + bgpixel] : display[index];
    display[index] = (spritepixel != 0) ? memory[0x3f10 + spritepallete*4 + spritepixel] : display[index];

    if (sprite0hit && !sprite0hit_hashappened) {
      ppustatus.spritezerohit = true;
      sprite0hit_hashappened = true;
    }
  }

  else if (0 <= currscanline && currscanline <= 239 && 321 <= currdot && currdot <= 336 && ppumask.bgenable) {
    pattern_data_hi_register.Fetch();
    pattern_data_lo_register.Fetch();
    pallete_hi_register.Fetch();
    pallete_lo_register.Fetch();
  }
}

void PPU::SpriteEvaluation() {
  

  if (currdot == 257) { // TODO: spread logic across cycles 65-256
    memset(secondary_oam, 0xff, 32);
    int num_sprites = 0;
    sprite0ind = -1;
    for (int sprite = 0; sprite < 64; sprite++) {
      byte curry = oam.Fetch(4*sprite);
      if (currscanline-7 <= curry && curry < currscanline+1) {
        secondary_oam[4*num_sprites + 0] = oam.Fetch(4*sprite + 0);
        secondary_oam[4*num_sprites + 1] = oam.Fetch(4*sprite + 1);
        secondary_oam[4*num_sprites + 2] = oam.Fetch(4*sprite + 2);
        secondary_oam[4*num_sprites + 3] = oam.Fetch(4*sprite + 3);

        if (sprite == 0) sprite0ind = num_sprites;

        if (++num_sprites == 8) break; // TODO: sprite overflow
      }
    }
  }
  else if (currdot == 340) {
    for (int i=0; i<8; i++) {
      byte spritey = secondary_oam[4*i + 0];
      byte spriteid = secondary_oam[4*i + 1];
      byte spriteattr = secondary_oam[4*i + 2];
      byte spritex = secondary_oam[4*i + 3];

      byte sy = spritey - currscanline + 7;

      if (!(spriteattr & 0x80)) sy = 7-sy;

      byte memlo = memory[16*spriteid + sy + 0x1000*!ppuctrl.bgtileselect];
      byte memhi = memory[16*spriteid + sy + 8 + 0x1000*!ppuctrl.bgtileselect];

      auto flip = [](byte *in) {
        const byte FLIPPED_NYBBLES[] = {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
        *in = FLIPPED_NYBBLES[*in >> 4] + 16*FLIPPED_NYBBLES[*in % 16];
      };

      if (spriteattr & 0x40) {
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
    byte ret;
    case PPUSTATUS:
      w = 0;
      ret = ppustatus.vblank*128 + ppustatus.spritezerohit*64 + ppustatus.spriteoverflow*32;
      if (ppustatus.vblank) ppustatus.vblank = false;
      if (ppustatus.spritezerohit) ppustatus.spritezerohit = false;
      if (ppustatus.spriteoverflow) ppustatus.spriteoverflow = false;
      return ret;
    case OAMDATA:
      return oam.Fetch();
    case PPUDATA:
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
      break;
    
    // TODO: OAM MMIO registers

    case OAMADDR:
      oam.oamaddr = val;
      break;

    case OAMDATA:
      oam.Write(val);
      break;
 
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
      }
      w = !w;
      break;

    case PPUDATA:
      if (!IsRendering()) {
        word add = v & ~0xc000;
        v += (ppuctrl.incrementmode) ? 32 : 1;

        
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

    case OAMDMA:
      oam.dmaaddr = val;
      oam.requestdma = true;
      break;
  }
}

