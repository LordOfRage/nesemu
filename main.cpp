#include "CPU.hpp"
#include "ROM.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include "debug.hpp"

int main() {
  CPU cpu;
  ROM *rom = new ROM("../../nestest.nes");

  cpu.Init(rom);

  int instructions = 9000;
  std::stringstream temp;
  std::string temps;
  while (instructions--) {
    temp << std::hex << cpu._debugPC();
    temps = temp.str();
    // for (auto &c : temps) c = toupper(c);

    debug << std::to_string(cpu._debugPC()) << "  ";
    
    cpu.Decode(cpu.FetchPC());
  }

  debug.close();
}
