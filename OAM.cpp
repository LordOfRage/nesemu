#include "OAM.hpp"
#include <stdio.h>

OAM::OAM() {
  cycles_left = 0;
}

byte OAM::Fetch(byte addr) {
  return memory[addr];
}

byte OAM::Fetch() {
  return memory[oamaddr];
}

void OAM::Write(byte addr, byte val) {
  memory[addr] = val; 
}

void OAM::Write(byte val) {
  memory[oamaddr] = val;
}

void OAM::DMA() {
  if (cycles_left == 0) return;

  if (cycles_left % 2 == 0) {
    memory[oamaddr] = dmamem[oamaddr];
    oamaddr++;
  }

  cycles_left--;
}
