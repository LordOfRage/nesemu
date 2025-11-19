#include "glew.h"
#include "glfw3.h"
#include "renderer.hpp"
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>

Renderer::Renderer(GLFWwindow *w, PPU &ppu) : ppu(ppu) {
  window = w;
}

Renderer::~Renderer() {

}

std::array<uint8_t, 256*240> &Renderer::GetPixelsAsTexture() {
  return ppu.display;
  // const uint8_t pallete[4] = {0x01, 0x16, 0x27, 0x18};
  //
  // for (int spritenum=0; spritenum<256; spritenum++) {
  //   for (int spritey=0; spritey<8; spritey++) {
  //     for (int spritex=0; spritex<8; spritex++) {
  //       int screenx = spritenum >> 4;
  //       int screeny = spritenum & 0xf;
  //       screenx *= 8;
  //       screeny *= 8;
  //
  //       int pixnum = screeny * 256 + screenx + spritey * 256 + spritex;
  //       debugdisplay[pixnum] = pallete[ppu.pattern_table_left[0x4000+spritenum*64+spritey*8+spritex]];
  //     }
  //   }
  // }
  //
  // return debugdisplay; // until PPU is implemented
}

void Renderer::InitTexture(std::array<uint8_t, 256*240>& buffer) {
  printf("TEXINIT");
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 256, 240, 0, GL_RED, GL_UNSIGNED_BYTE, buffer.data());
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
