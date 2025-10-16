#pragma once
#include <array>
#include <cstdint>
#include "ROM.hpp"
#define byte uint8_t
#define word uint16_t
#define sbyte int8_t
#define sword int16_t

class CPU {
public:
  CPU(ROM&);
  ~CPU();

  byte Fetch(word);
  byte Fetch(word, byte);
  word FetchWord(word, byte = 0);
  byte FetchPC();
  byte FetchPC(byte);
  word FetchWordPC();

  void Decode(byte);

  bool GetFlag(byte flag) { return (processor_flags & flag) != 0; }
  void SetFlag(byte flag) { processor_flags |= flag; }
  void ClearFlag(byte flag) { processor_flags &= ~flag; }
  void WriteFlag(byte flag, bool val) { val ? SetFlag(flag) : ClearFlag(flag); }

  void Write(word, byte);
  void Push(byte val) { memory[0x100 + sp--] = val; }
  byte Pull() { return memory[0x100 + ++sp]; }

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
  byte accumulator, x, y, sp, processor_flags;
  word pc;

  ROM &rom;
  
  const byte C = 1 << 0;
  const byte Z = 1 << 1;
  const byte I = 1 << 2;
  const byte D = 1 << 3;
  const byte B = 1 << 4;
  //const byte _ = 1 << 5;  This one is ignored
  const byte V = 1 << 6;
  const byte N = 1 << 7;

};
