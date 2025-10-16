#pragma once
#include "ROM.hpp"
#include <array>
#include <cstdint>

class PPU {
public:
  PPU(ROM&);
  ~PPU();

private:
  std::array<uint8_t, 0x4000> memory;
};
