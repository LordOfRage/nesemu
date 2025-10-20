#include "PPU.hpp"
#include <cstdint>
#include <fstream>

PPU::PPU(ROM &r) : rom(r) {
  printf("%s\n", rom.filename.c_str());
  std::ifstream file("../../nestest.nes", std::ios::binary);
  file.seekg(0x10 + 0x4000*rom.header.v1.prgromunits);

  for (int i=0; i<0x100*rom.header.v1.chrromunits; i++) {
    uint8_t buff[16];
    for (int j=0; j<16; j++) {
      buff[j] = file.get();
    }


    for (int y=0; y<8; y++) {
      for (int x=0; x<8; x++) {
        pattern_table_sprites.push_back((bool)(buff[y] & (1 >> x)) + 2*(bool)(buff[y+8] & (1 >> x)));
      }
    }
  }

  for (int i=0; i<pattern_table_sprites.size()/16; i += 16) {
    for (int j=0; j<16; j++) printf("%x ", pattern_table_sprites[i+j]);
    printf("\n");
  }

  file.close();
}

PPU::~PPU() {

}
