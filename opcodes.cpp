#include "CPU.hpp"
#include "debug.hpp"
#include <ranges>

enum Opcodes {
  BRKimpl = 0x0,
  ORAxind = 0x1,
  ORAzpg = 0x5,
  ASLzpg = 0x6,
  PHPimpl = 0x8,
  ORAimm = 0x9,
  ASLa = 0xa,
  ORAabs = 0xd,
  ASLabs = 0xe,
  BPLrel = 0x10,
  ORAindy = 0x11,
  ORAzpgx = 0x15,
  ASLzpgx = 0x16,
  CLCimpl = 0x18,
  ORAabsy = 0x19,
  ORAabsx = 0x1d,
  ASLabsx = 0x1e,
  JSRabs = 0x20,
  ANDxind = 0x21,
  BITzpg = 0x24,
  ANDzpg = 0x25,
  ROLzpg = 0x26,
  PLPimpl = 0x28,
  ANDimm = 0x29,
  ROLa = 0x2a,
  BITabs = 0x2c,
  ANDabs = 0x2d,
  ROLabs = 0x2e,
  BMIrel = 0x30,
  ANDindy = 0x31,
  ANDzpgx = 0x35,
  ROLzpgx = 0x36,
  SECimpl = 0x38,
  ANDabsy = 0x39,
  ANDabsx = 0x3d,
  ROLabsx = 0x3e,
  RTIimpl = 0x40,
  EORxind = 0x41,
  EORzpg = 0x45,
  LSRzpg = 0x46,
  PHAimpl = 0x48,
  EORimm = 0x49,
  LSRa = 0x4a,
  JMPabs = 0x4c,
  EORabs = 0x4d,
  LSRabs = 0x4e,
  BVCrel = 0x50,
  EORindy = 0x51,
  EORzpgx = 0x55,
  LSRzpgx = 0x56,
  CLIimpl = 0x58,
  EORabsy = 0x59,
  EORabsx = 0x5d,
  LSRabsx = 0x5e,
  RTSimpl = 0x60,
  ADCxind = 0x61,
  ADCzpg = 0x65,
  RORzpg = 0x66,
  PLAimpl = 0x68,
  ADCimm = 0x69,
  RORa = 0x6a,
  JMPind = 0x6c,
  ADCabs = 0x6d,
  RORabs = 0x6e,
  BVSrel = 0x70,
  ADCindy = 0x71,
  ADCzpgx = 0x75,
  RORzpgx = 0x76,
  SEIimpl = 0x78,
  ADCabsy = 0x79,
  ADCabsx = 0x7d,
  RORabsx = 0x7e,
  STAxind = 0x81,
  STYzpg = 0x84,
  STAzpg = 0x85,
  STXzpg = 0x86,
  DEYimpl = 0x88,
  TXAimpl = 0x8a,
  STYabs = 0x8c,
  STAabs = 0x8d,
  STXabs = 0x8e,
  BCCrel = 0x90,
  STAindy = 0x91,
  STYzpgx = 0x94,
  STAzpgx = 0x95,
  STXzpgy = 0x96,
  TYAimpl = 0x98,
  STAabsy = 0x99,
  TXSimpl = 0x9a,
  STAabsx = 0x9d,
  LDYimm = 0xa0,
  LDAxind = 0xa1,
  LDXimm = 0xa2,
  LDYzpg = 0xa4,
  LDAzpg = 0xa5,
  LDXzpg = 0xa6,
  TAYimpl = 0xa8,
  LDAimm = 0xa9,
  TAXimpl = 0xaa,
  LDYabs = 0xac,
  LDAabs = 0xad,
  LDXabs = 0xae,
  BCSrel = 0xb0,
  LDAindy = 0xb1,
  LDYzpgx = 0xb4,
  LDAzpgx = 0xb5,
  LDXzpgy = 0xb6,
  CLVimpl = 0xb8,
  LDAabsy = 0xb9,
  TSXimpl = 0xba,
  LDYabsx = 0xbc,
  LDAabsx = 0xbd,
  LDXabsy = 0xbe,
  CPYimm = 0xc0,
  CMPxind = 0xc1,
  CPYzpg = 0xc4,
  CMPzpg = 0xc5,
  DECzpg = 0xc6,
  INYimpl = 0xc8,
  CMPimm = 0xc9,
  DEXimpl = 0xca,
  CPYabs = 0xcc,
  CMPabs = 0xcd,
  DECabs = 0xce,
  BNErel = 0xd0,
  CMPindy = 0xd1,
  CMPzpgx = 0xd5,
  DECzpgx = 0xd6,
  CLDimpl = 0xd8,
  CMPabsy = 0xd9,
  CMPabsx = 0xdd,
  DECabsx = 0xde,
  CPXimm = 0xe0,
  SBCxind = 0xe1,
  CPXzpg = 0xe4,
  SBCzpg = 0xe5,
  INCzpg = 0xe6,
  INXimpl = 0xe8,
  SBCimm = 0xe9,
  NOPimpl = 0xea,
  CPXabs = 0xec,
  SBCabs = 0xed,
  INCabs = 0xee,
  BEQrel = 0xf0,
  SBCindy = 0xf1,
  SBCzpgx = 0xf5,
  INCzpgx = 0xf6,
  SEDimpl = 0xf8,
  SBCabsy = 0xf9,
  SBCabsx = 0xfd,
  INCabsx = 0xfe,
};

void CPU::Decode(byte opcode) {
  if (pc == 0x8e97) {
    // getchar();
  }

  if (pc == 0x80c4) {
    word addr = memory[0] + memory[1] * 256;
    debug << to_hex(memory[0] + memory[1] * 256) << "\n";
    if (addr < 0x8000) {
      for (int i=0; i<0x200; i += 16) {
        for (int j=0; j<16; j++) {
          debug << to_hex(memory[0x300+i+j]) << " ";
        }
        debug << "\n";
      }
    }
    // getchar();
  }

  switch(opcode) {

    case ADCxind:
      ADC(Fetch(FetchWord(FetchPC(),x)));
      break;
    case ADCzpg:
      ADC(Fetch(FetchPC()));
      break;
    case ADCimm:
      ADC(FetchPC());
      break;
    case ADCabs:
      ADC(Fetch(FetchWordPC()));
      break;
    case ADCindy:
      ADC(Fetch(FetchWord(FetchPC())+y));
      break;
    case ADCzpgx:
      ADC(Fetch(FetchPC(),x));
      break;
    case ADCabsy:
      ADC(Fetch(FetchWordPC()+y));
      break;
    case ADCabsx:
      ADC(Fetch(FetchWordPC()+x));
      break;

    case ANDxind:
      AND(Fetch(FetchWord(FetchPC(),x)));
      break;
    case ANDzpg:
      AND(Fetch(FetchPC()));
      break;
    case ANDimm:
      AND(FetchPC());
      break;
    case ANDabs:
      AND(Fetch(FetchWordPC()));
      break;
    case ANDindy:
      AND(Fetch(FetchWord(FetchPC())+y));
      break;
    case ANDzpgx:
      AND(Fetch(FetchPC(),x));
      break;
    case ANDabsy:
      AND(Fetch(FetchWordPC()+y));
      break;
    case ANDabsx:
      AND(Fetch(FetchWordPC()+x));
      break;

    case ASLzpg:
      ASL(FetchPC());
      break;
    case ASLa:
      ASL();
      break;
    case ASLabs:
      ASL(FetchWordPC());
      break;
    case ASLzpgx:
      ASL(FetchPC()+x);
      break;
    case ASLabsx:
      ASL(FetchWordPC()+x);
      break;

    case BCCrel:
      BCC(FetchPC());
      break;

    case BCSrel:
      BCS(FetchPC());
      break;

    case BEQrel:
      BEQ(FetchPC());
      break;

    case BITzpg:
      BIT(Fetch(FetchPC()));
      break;
    case BITabs:
      BIT(Fetch(FetchWordPC()));
      break;

    case BMIrel:
      BMI(FetchPC());
      break;

    case BNErel:
      BNE(FetchPC());
      break;

    case BPLrel:
      BPL(FetchPC());
      break;

    case BRKimpl:
      BRK();
      break;

    case BVCrel:
      BVC(FetchPC());
      break;

    case BVSrel:
      BVS(FetchPC());
      break;

    case CLCimpl:
      CLC();
      break;

    case CLDimpl:
      CLD();
      break;

    case CLIimpl:
      CLI();
      break;

    case CLVimpl:
      CLV();
      break;

    case CMPxind:
      CMP(Fetch(FetchWord(FetchPC(),x)));
      break;
    case CMPzpg:
      CMP(Fetch(FetchPC()));
      break;
    case CMPimm:
      CMP(FetchPC());
      break;
    case CMPabs:
      CMP(Fetch(FetchWordPC()));
      break;
    case CMPindy:
      CMP(Fetch(FetchWord(FetchPC())+y));
      break;
    case CMPzpgx:
      CMP(Fetch(FetchPC(),x));
      break;
    case CMPabsy:
      CMP(Fetch(FetchWordPC()+y));
      break;
    case CMPabsx:
      CMP(Fetch(FetchWordPC()+x));
      break;

    case CPXimm:
      CPX(FetchPC());
      break;
    case CPXzpg:
      CPX(Fetch(FetchPC()));
      break;
    case CPXabs:
      CPX(Fetch(FetchWordPC()));
      break;

    case CPYimm:
      CPY(FetchPC());
      break;
    case CPYzpg:
      CPY(Fetch(FetchPC()));
      break;
    case CPYabs:
      CPY(Fetch(FetchWordPC()));
      break;

    case DECzpg:
      DEC(FetchPC());
      break;
    case DECabs:
      DEC(FetchWordPC());
      break;
    case DECzpgx:
      DEC(FetchPC()+x);
      break;
    case DECabsx:
      DEC(FetchWordPC()+x);
      break;

    case DEXimpl:
      DEX();
      break;

    case DEYimpl:
      DEY();
      break;

    case EORxind:
      EOR(Fetch(FetchWord(FetchPC(),x)));
      break;
    case EORzpg:
      EOR(Fetch(FetchPC()));
      break;
    case EORimm:
      EOR(FetchPC());
      break;
    case EORabs:
      EOR(Fetch(FetchWordPC()));
      break;
    case EORindy:
      EOR(Fetch(FetchWord(FetchPC())+y));
      break;
    case EORzpgx:
      EOR(Fetch(FetchPC(),x));
      break;
    case EORabsy:
      EOR(Fetch(FetchWordPC()+y));
      break;
    case EORabsx:
      EOR(Fetch(FetchWordPC()+x));
      break;

    case INCzpg:
      INC(FetchPC());
      break;
    case INCabs:
      INC(FetchWordPC());
      break;
    case INCzpgx:
      INC(FetchPC()+x);
      break;
    case INCabsx:
      INC(FetchWordPC()+x);
      break;

    case INXimpl:
      INX();
      break;

    case INYimpl:
      INY();
      break;

    case JMPabs:
      JMP(FetchWordPC());
      break;
    case JMPind:
      JMP(FetchWord(FetchWordPC()));
      break;

    case JSRabs:
      JSR(FetchWordPC());
      break;

    case LDAxind:
      LDA(Fetch(FetchWord(FetchPC(),x)));
      break;
    case LDAzpg:
      LDA(Fetch(FetchPC()));
      break;
    case LDAimm:
      LDA(FetchPC());
      break;
    case LDAabs:
      LDA(Fetch(FetchWordPC()));
      break;
    case LDAindy:
      LDA(Fetch(FetchWord(FetchPC())+y));
      break;
    case LDAzpgx:
      LDA(Fetch(FetchPC(),x));
      break;
    case LDAabsy:
      LDA(Fetch(FetchWordPC()+y));
      break;
    case LDAabsx:
      LDA(Fetch(FetchWordPC()+x));
      break;

    case LDXimm:
      LDX(FetchPC());
      break;
    case LDXzpg:
      LDX(Fetch(FetchPC()));
      break;
    case LDXabs:
      LDX(Fetch(FetchWordPC()));
      break;
    case LDXzpgy:
      LDX(Fetch(FetchPC(),y));
      break;
    case LDXabsy:
      LDX(Fetch(FetchWordPC()+y));
      break;

    case LDYimm:
      LDY(FetchPC());
      break;
    case LDYzpg:
      LDY(Fetch(FetchPC()));
      break;
    case LDYabs:
      LDY(Fetch(FetchWordPC()));
      break;
    case LDYzpgx:
      LDY(Fetch(FetchPC(),x));
      break;
    case LDYabsx:
      LDY(Fetch(FetchWordPC()+x));
      break;

    case LSRzpg:
      LSR(FetchPC());
      break;
    case LSRa:
      LSR();
      break;
    case LSRabs:
      LSR(FetchWordPC());
      break;
    case LSRzpgx:
      LSR(FetchPC()+x);
      break;
    case LSRabsx:
      LSR(FetchWordPC()+x);
      break;

    case NOPimpl:
      NOP();
      break;

    case ORAxind:
      ORA(Fetch(FetchWord(FetchPC(),x)));
      break;
    case ORAzpg:
      ORA(Fetch(FetchPC()));
      break;
    case ORAimm:
      ORA(FetchPC());
      break;
    case ORAabs:
      ORA(Fetch(FetchWordPC()));
      break;
    case ORAindy:
      ORA(Fetch(FetchWord(FetchPC())+y));
      break;
    case ORAzpgx:
      ORA(Fetch(FetchPC(),x));
      break;
    case ORAabsy:
      ORA(Fetch(FetchWordPC()+y));
      break;
    case ORAabsx:
      ORA(Fetch(FetchWordPC()+x));
      break;

    case PHAimpl:
      PHA();
      break;

    case PHPimpl:
      PHP();
      break;

    case PLAimpl:
      PLA();
      break;

    case PLPimpl:
      PLP();
      break;

    case ROLzpg:
      ROL(FetchPC());
      break;
    case ROLa:
      ROL();
      break;
    case ROLabs:
      ROL(FetchWordPC());
      break;
    case ROLzpgx:
      ROL(FetchPC()+x);
      break;
    case ROLabsx:
      ROL(FetchWordPC()+x);
      break;

    case RORzpg:
      ROR(FetchPC());
      break;
    case RORa:
      ROR();
      break;
    case RORabs:
      ROR(FetchWordPC());
      break;
    case RORzpgx:
      ROR(FetchPC()+x);
      break;
    case RORabsx:
      ROR(FetchWordPC()+x);
      break;

    case RTIimpl:
      RTI();
      break;

    case RTSimpl:
      RTS();
      break;

    case SBCxind:
      SBC(Fetch(FetchWord(FetchPC(),x)));
      break;
    case SBCzpg:
      SBC(Fetch(FetchPC()));
      break;
    case SBCimm:
      SBC(FetchPC());
      break;
    case SBCabs:
      SBC(Fetch(FetchWordPC()));
      break;
    case SBCindy:
      SBC(Fetch(FetchWord(FetchPC())+y));
      break;
    case SBCzpgx:
      SBC(Fetch(FetchPC(),x));
      break;
    case SBCabsy:
      SBC(Fetch(FetchWordPC()+y));
      break;
    case SBCabsx:
      SBC(Fetch(FetchWordPC()+x));
      break;

    case SECimpl:
      SEC();
      break;

    case SEDimpl:
      SED();
      break;

    case SEIimpl:
      SEI();
      break;

    case STAxind:
      STA(FetchWord(FetchPC(),x));
      break;
    case STAzpg:
      STA(FetchPC());
      break;
    case STAabs:
      STA(FetchWordPC());
      break;
    case STAindy:
      STA(FetchWord(FetchPC())+y);
      break;
    case STAzpgx:
      STA(FetchPC()+x);
      break;
    case STAabsy:
      STA(FetchWordPC()+y);
      break;
    case STAabsx:
      STA(FetchWordPC()+x);
      break;

    case STXzpg:
      STX(FetchPC());
      break;
    case STXabs:
      STX(FetchWordPC());
      break;
    case STXzpgy:
      STX(FetchPC(),y);
      break;

    case STYzpg:
      STY(FetchPC());
      break;
    case STYabs:
      STY(FetchWordPC());
      break;
    case STYzpgx:
      STY(FetchPC()+x);
      break;

    case TAXimpl:
      TAX();
      break;

    case TAYimpl:
      TAY();
      break;

    case TSXimpl:
      TSX();
      break;

    case TXAimpl:
      TXA();
      break;

    case TXSimpl:
      TXS();
      break;

    case TYAimpl:
      TYA();
      break;
  }

  ppu.PerformCycles(cycles_passed*3);
  cycles_passed = 0;

  if (ppu.nmitrigger) TriggerNMI();
}
