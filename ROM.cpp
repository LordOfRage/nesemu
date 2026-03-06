#include "ROM.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>

ROM::ROM(std::string filename) : filename(filename) {
  std::ifstream file(filename, std::ios::binary);

  // get the header
  for (int_fast8_t i=0; i<16; i++) header.v1.header[i] = file.get();

  // compare the first 4 bytes to their expected values for a .nes file, and 
  // error if they differ
  byte expected_nes[4] = {'N', 'E', 'S', 0x1a};
  if (memcmp(header.v1.NES, expected_nes, 4)) throw std::invalid_argument("Not valid NES ROM!");

  // if there's only one unit of program ROM
  if (header.v1.prgromunits == 1) {
    memory = (byte *)malloc(2 * 0x4000);
    for (int i=0; i<0x4000; i++) {
      memory[i] = file.get();
      memory[i+0x4000] = memory[i];
    }

    file.close();
    return;
  }
  
  // if there's more than one unit of program ROM
  memory = (byte *)malloc(header.v1.prgromunits * 0x4000);
  for (int i=0; i<header.v1.prgromunits * 0x4000; i++) {
    memory[i] = file.get();
  }

  file.close();
}

