#pragma once
#include "PPU.hpp"
#include "glfw3.h"
#include <cstdint>
#include <array>

class Renderer {
public:
  Renderer(GLFWwindow*, PPU&);
  ~Renderer();

  std::array<uint8_t, PPU::AREA_PIXELS> &GetPixelsAsTexture();
  void InitTexture(std::array<uint8_t, PPU::AREA_PIXELS>&);
  void BindTexture();
  void UnbindTexture();

private:
  GLFWwindow *window;
  PPU &ppu;
  
  unsigned int textureID; // OpenGL texture ID
};
