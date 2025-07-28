#include "CPU.hpp"
#include "debug.hpp"
#include <numeric>

byte HiByte(word val) {
  return val >> 8;
}

byte LoByte(word val) {
  return val;
}

void CPU::ADC(byte val) {
  byte res = accumulator + val + (byte)GetFlag(C);

  WriteFlag(C, accumulator + val + (byte)GetFlag(C) >= 0x100);
  WriteFlag(Z, res == 0);
  WriteFlag(N, (res & N) != 0);
  WriteFlag(V, (accumulator + val + GetFlag(C) < 0) != (bool)((sbyte)res & N));

  accumulator = res;
}

void CPU::AND(byte val) {
  byte res = accumulator & val;

  WriteFlag(Z, res == 0);
  WriteFlag(N, (res & N) != 0);

  accumulator = res;
}

void CPU::ASL(word addr) {
  byte val = Fetch(addr);
  bool temp = ((val & N) != 0);
  Write(addr, (val << 1));
  WriteFlag(C, temp);
}

void CPU::ASL() {
  bool temp = ((accumulator & N) != 0);
  accumulator = (accumulator << 1);
  WriteFlag(C, temp);
}

void CPU::BCC(sbyte offset) {
  if (!GetFlag(C)) pc += offset;
}

void CPU::BCS(sbyte offset) {
  if (GetFlag(C)) pc += offset;
}

void CPU::BEQ(sbyte offset) {
  if (GetFlag(Z)) pc += offset;
}

void CPU::BIT(byte val) {
  WriteFlag(N, (val & N) != 0);
  WriteFlag(V, (val & V) != 0);
  WriteFlag(Z, (val & accumulator) == 0);
}

void CPU::BMI(sbyte offset) {
  if (GetFlag(N)) pc += offset;
}

void CPU::BNE(sbyte offset) {
  if (!GetFlag(Z)) pc += offset;
}

void CPU::BPL(sbyte offset) {
  if (!GetFlag(N)) pc += offset;
}

void CPU::BRK() {
  Push(HiByte(pc));
  Push(LoByte(pc));
  Push(processor_flags | B);
  pc = FetchWord(0xfffa);
}

void CPU::BVC(sbyte offset) {
  if (!GetFlag(V)) pc += offset;
}

void CPU::BVS(sbyte offset) {
  if (GetFlag(V)) pc += offset;
}

void CPU::CLC() {
  ClearFlag(C);
}

void CPU::CLD() {
  ClearFlag(D);
}

void CPU::CLI() {
  ClearFlag(I);
}

void CPU::CLV() {
  ClearFlag(V);
}

void CPU::CMP(byte val) {
  WriteFlag(C, accumulator >= val);
  WriteFlag(Z, accumulator == val);
  WriteFlag(N, (((accumulator - val) & N) != 0));
}

void CPU::CPX(byte val) {
  WriteFlag(C, x >= val);
  WriteFlag(Z, x == val);
  WriteFlag(N, (((x - val) & N) != 0));
}

void CPU::CPY(byte val) {
  WriteFlag(C, y >= val);
  WriteFlag(Z, y == val);
  WriteFlag(N, (((y - val) & N) != 0));
}

void CPU::DEC(word addr) {
  Write(addr, Fetch(addr)-1);
}

void CPU::DEX() {
  x--;
}

void CPU::DEY() {
  y--;
}

void CPU::EOR(byte val) {
  accumulator ^= val;
}

void CPU::INC(word addr) {
  Write(addr, Fetch(addr)+1);
}

void CPU::INX() {
  x++;
}

void CPU::INY() {
  y++;
}

void CPU::JMP(word addr) {
  pc = addr;
}

void CPU::JSR(word addr) {
  Push(HiByte(pc - 1));
  Push(LoByte(pc - 1));
  JMP(addr);
}

void CPU::LDA(byte val) {
  accumulator = val;
}

void CPU::LDX(byte val) {
  x = val;
}

void CPU::LDY(byte val) {
  y = val;
}

void CPU::LSR(word addr) {
  byte val = Fetch(addr);
  bool temp = ((val & 1) != 0);
  Write(addr, (val >> 1));
  WriteFlag(C, temp);
}

void CPU::LSR() {
  bool temp = ((accumulator & 1) != 0);
  accumulator = (accumulator >> 1);
  WriteFlag(C, temp);
}

void CPU::NOP() {
  WaitCycle();
}

void CPU::ORA(byte val) {
  accumulator |= val;
}

void CPU::PHA() {
  Push(accumulator);
}

void CPU::PHP() {
  Push(processor_flags | B);
}

void CPU::PLA() {
  accumulator = Pull();
}

void CPU::PLP() {
  processor_flags = Pull() & ~B;
}

void CPU::ROL(word addr) {
  byte val = Fetch(addr);
  bool temp = ((val & N) != 0);
  Write(addr, (val << 1) + GetFlag(C));
  WriteFlag(C, temp);
}

void CPU::ROL() {
  bool temp = ((accumulator & N) != 0);
  accumulator = (accumulator << 1) + GetFlag(C);
  WriteFlag(C, temp);
}

void CPU::ROR(word addr) {
  byte val = Fetch(addr);
  bool temp = ((val & 1) != 0);
  Write(addr, (val >> 1) + N*GetFlag(C));
  WriteFlag(C, temp);
}

void CPU::ROR() {
  bool temp = ((accumulator & 1) != 0);
  accumulator = (accumulator >> 1) + N*GetFlag(C);
  WriteFlag(C, temp);
}

void CPU::RTI() {
  processor_flags = Pull() & ~B;
  pc = Pull() + 256*Pull();
}

void CPU::RTS() {
  pc = Pull() + 256*Pull() + 1;
}

void CPU::SBC(byte val) {
  byte res = accumulator - val - !GetFlag(C);
  WriteFlag(C, val + !GetFlag(C) > accumulator);
  WriteFlag(Z, res == 0);
  WriteFlag(V, (accumulator - val - !GetFlag(C) < 0) != (bool)((sbyte)res & N));
}

void CPU::SEC() {
  SetFlag(C);
}

void CPU::SED() {
  SetFlag(D);
}

void CPU::SEI() {
  SetFlag(I);
}

void CPU::STA(word addr) {
  Write(addr, accumulator);
}

void CPU::STX(word addr) {
  Write(addr, x);
}

void CPU::STY(word addr) {
  Write(addr, y);
}

void CPU::TAX() {
  x = accumulator;
}

void CPU::TAY() {
  y = accumulator;
}

void CPU::TSX() {
  x = sp;
}

void CPU::TXA() {
  accumulator = x;
}

void CPU::TXS() {
  sp = x;
}

void CPU::TYA() {
  accumulator = y;
}
