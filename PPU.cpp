#include "PPU.hpp"
#include <cstdint>
#include <fstream>

PPU::PPU(ROM &r) : rom(r) {
  std::ifstream file("../../nestest.nes", std::ios::binary); // TODO: figure out why rom.filename reference not working
  file.seekg(0x10 + 0x4000*rom.header.v1.prgromunits);

  for (int i=0; i<0x100*rom.header.v1.chrromunits; i++) {
    uint8_t buff[16];
    for (int j=0; j<16; j++) {
      buff[j] = file.get();
      // printf("%x ", buff[j]);
    }


    for (int y=0; y<8; y++) {
      for (int x=0; x<8; x++) {
        pattern_table_sprites.push_back((bool)(buff[y] & (0x80 >> x)) + 2*(bool)(buff[y+8] & (0x80 >> x)));
        // printf("%x ", pattern_table_sprites[pattern_table_sprites.size()-1]);
      }
      // printf("\n");
    }
    printf("\n");
  }

  file.close();
}

PPU::~PPU() {

}
