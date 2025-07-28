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
    a = cpu.accumulator;
    x = cpu.x;
    y = cpu.y;
    p = cpu.processor_flags;
    sp = cpu.sp;
    pc = cpu.pc;
    
    debug << "" << to_hex<word>(pc) << "  ";
    cpu.Decode(cpu.FetchPC());
    debug << "       A:" << to_hex(a) << " X:" << to_hex(x) << " Y:" << to_hex(y) << " P:" << to_hex(p) << " SP:" << to_hex(sp);
    debug << std::endl;
  }

  debug.close();
}
