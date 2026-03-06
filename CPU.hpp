#pragma once
#include <array>
#include <cstdint>
#include "ROM.hpp"
#include "PPU.hpp"
#include "controller.hpp"
#define byte uint8_t
#define word uint16_t
#define sbyte int8_t
#define sword int16_t

class CPU {
public:
  CPU(ROM&, PPU&, Controller&);

  byte Fetch(word);
  word FetchWord(word);
  byte FetchPC();
  word FetchWordPC();

  // methods to fetch with a byte offset, if the byte offset shouldn't cause an overflow in the
  // high byte of the address
  byte Fetch(word, byte);
  word FetchWord(word, byte);
  byte FetchPC(byte);

  void Decode(byte);

  // helper methods to read/write processor flags: "flag" variable should be one of C, Z, I, B, D, V, N
  bool GetFlag(byte flag) { return (processor_flags & flag) != 0; }
  void SetFlag(byte flag) { processor_flags |= flag; }
  void ClearFlag(byte flag) { processor_flags &= ~flag; }
  void WriteFlag(byte flag, bool val) { val ? SetFlag(flag) : ClearFlag(flag); }

  void Write(word, byte);
  void Push(byte val) { WaitCycle(); memory[0x100 + stack_pointer--] = val; }
  byte Pull() { WaitCycle(); return memory[0x100 + ++stack_pointer]; }

  void WaitCycle();

  void ADC(byte);
  void AND(byte);
  void ASL(word);
  void ASL();
  void BCC(sbyte);
  void BCS(sbyte);
  void BEQ(sbyte);
  void BIT(byte);
  void BMI(sbyte);
  void BNE(sbyte);
  void BPL(sbyte);
  void BRK();
  void BVC(sbyte);
  void BVS(sbyte);
  void CLC();
  void CLD();
  void CLI();
  void CLV();
  void CMP(byte);
  void CPX(byte);
  void CPY(byte);
  void DEC(word);
  void DEX();
  void DEY();
  void EOR(byte);
  void INC(word);
  void INX();
  void INY();
  void JMP(word);
  void JSR(word);
  void LDA(byte);
  void LDX(byte);
  void LDY(byte);
  void LSR(word);
  void LSR();
  void NOP();
  void ORA(byte);
  void PHA();
  void PHP();
  void PLA();
  void PLP();
  void ROL(word);
  void ROL();
  void ROR(word);
  void ROR();
  void RTI();
  void RTS();
  void SBC(byte);
  void SEC();
  void SED();
  void SEI();
  void STA(word);
  void STX(word);
  void STX(word, byte);
  void STY(word);
  void TAX();
  void TAY();
  void TSX();
  void TXA();
  void TXS();
  void TYA();

private:
  std::array<byte, 0x8000> memory;
  word pc;
  byte accumulator, x_register, y_register, stack_pointer, processor_flags;

  byte cycles_passed; // CPU cycles taken up by current instruction

  ROM &rom;
  PPU &ppu;
  Controller &controller;
  void TriggerNMI();
  void CheckDMA();
  void TickDMA();
  
  // processor flags locations within the processor_flags byte
  // for example, the Z flag is the 2nd most significant bit, so
  // it has the value 0b0000 0010
  const byte C = 1 << 0;
  const byte Z = 1 << 1;
  const byte I = 1 << 2;
  const byte D = 1 << 3;
  const byte B = 1 << 4;
  //const byte _ = 1 << 5;  This one is ignored
  const byte V = 1 << 6;
  const byte N = 1 << 7;

};
