#include "glew.h"
#include "glfw3.h"
#include "renderer.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>

Renderer::Renderer(GLFWwindow *w) {
  window = w;
}

Renderer::~Renderer() {

}

std::array<uint8_t, 256*240> &Renderer::GetPixelsAsTexture() {
  std::ifstream rom("../../smb.nes", std::ios::binary);
  rom.seekg(0x8010);

  rom.seekg(0x8010);
  uint8_t pallete[4] = {0x01, 0x16, 0x27, 0x18};

  for (int k=0; k<30; k++) {
    for (int i=0; i<32; i++) {
      uint8_t buff[16];
      for (int j=0; j<16; j++) {
        buff[j] = rom.get();
      }

      for (int y=0; y<8; y++) {
        for (int x=0; x<8; x++) {
          debugdisplay[256 * (k*8+y) + (i*8+x)] = (pallete[(bool)(buff[y] & (0x80 >> x)) + 2*(bool)(buff[y+8] & (0x80 >> x))]);
        }
      }
    }
  }

  for (int i=0; i<256*240; i++) {
  }

  rom.close();
  return debugdisplay; // until PPU is implemented
}

void Renderer::InitTexture(std::array<uint8_t, 256*240>& buffer) {
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
