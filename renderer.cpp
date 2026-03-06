#include "glew.h"
#include "glfw3.h"
#include "renderer.hpp"
#include <array>
#include <cstdint>

Renderer::Renderer(GLFWwindow *w, PPU &ppu) : ppu(ppu) {
  window = w;
}

Renderer::~Renderer() {

}

std::array<uint8_t, PPU::AREA_PIXELS> &Renderer::GetPixelsAsTexture() {
  return ppu.display;
}

void Renderer::InitTexture(std::array<uint8_t, PPU::AREA_PIXELS>& buffer) {
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, PPU::WIDTH_PIXELS, PPU::HEIGHT_PIXELS, 0, GL_RED, GL_UNSIGNED_BYTE, buffer.data());
}

void Renderer::BindTexture() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
}

void Renderer::UnbindTexture() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(1, &textureID);
}
