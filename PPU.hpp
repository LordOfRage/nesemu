#pragma once
#include "ROM.hpp"
#include <array>
#include <cstdint>
#include <vector>

class PPU {
public:
  PPU(ROM&);
  ~PPU();

private:
  std::array<uint8_t, 0x4000> memory;
  std::vector<uint8_t> pattern_table_sprites;
  std::vector<uint8_t> pattern_table_bg;

  ROM &rom;
};
