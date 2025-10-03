#include "CPU.hpp"
#include "ROM.hpp"
#include <ostream>
#include <string>
#include "debug.hpp"

int main() {
  CPU cpu;
  ROM *rom = new ROM("../../nestest.nes");

  cpu.Init(rom);
  byte a, x, y, p, sp;
  word pc;

  int instructions = 9000;
  while (instructions--) {
    cpu.Decode(cpu.FetchPC());
  }

  debug.close();
}
