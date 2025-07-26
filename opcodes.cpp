#include "CPU.hpp"

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
  switch(opcode) {

    case ADCxind:
      ADC(Fetch(FetchWord(FetchWordPC()+x)));
    case ADCzpg:
      ADC(Fetch(FetchPC()));
    case ADCimm:
      ADC(FetchPC());
    case ADCabs:
      ADC(Fetch(FetchWordPC()));
    case ADCindy:
      ADC(Fetch(FetchWord(FetchWordPC())+y));
    case ADCzpgx:
      ADC(Fetch(FetchPC()+x));
    case ADCabsy:
      ADC(Fetch(FetchWordPC()+y));
    case ADCabsx:
      ADC(Fetch(FetchWordPC()+x));

    case ANDxind:
      AND(Fetch(FetchWord(FetchWordPC()+x)));
    case ANDzpg:
      AND(Fetch(FetchPC()));
    case ANDimm:
      AND(FetchPC());
    case ANDabs:
      AND(Fetch(FetchWordPC()));
    case ANDindy:
      AND(Fetch(FetchWord(FetchWordPC())+y));
    case ANDzpgx:
      AND(Fetch(FetchPC()+x));
    case ANDabsy:
      AND(Fetch(FetchWordPC()+y));
    case ANDabsx:
      AND(Fetch(FetchWordPC()+x));

    case ASLzpg:
      ASL(FetchPC());
    case ASLa:
      ASL();
    case ASLabs:
      ASL(FetchWordPC());
    case ASLzpgx:
      ASL(FetchPC()+x);
    case ASLabsx:
      ASL(FetchWordPC()+x);

    case BCCrel:
      BCC(FetchPC());

    case BCSrel:
      BCS(FetchPC());

    case BEQrel:
      BEQ(FetchPC());

    case BITzpg:
      BIT(Fetch(FetchPC()));
    case BITabs:
      BIT(Fetch(FetchWordPC()));

    case BMIrel:
      BMI(FetchPC());

    case BNErel:
      BNE(FetchPC());

    case BPLrel:
      BPL(FetchPC());

    case BRKimpl:
      BRK();

    case BVCrel:
      BVC(FetchPC());

    case BVSrel:
      BVS(FetchPC());

    case CLCimpl:
      CLC();

    case CLDimpl:
      CLD();

    case CLIimpl:
      CLI();

    case CLVimpl:
      CLV();

    case CMPxind:
      CMP(Fetch(FetchWord(FetchWordPC()+x)));
    case CMPzpg:
      CMP(Fetch(FetchPC()));
    case CMPimm:
      CMP(FetchPC());
    case CMPabs:
      CMP(Fetch(FetchWordPC()));
    case CMPindy:
      CMP(Fetch(FetchWord(FetchWordPC())+y));
    case CMPzpgx:
      CMP(Fetch(FetchPC()+x));
    case CMPabsy:
      CMP(Fetch(FetchWordPC()+y));
    case CMPabsx:
      CMP(Fetch(FetchWordPC()+x));

    case CPXimm:
      CPX(FetchPC());
    case CPXzpg:
      CPX(Fetch(FetchPC()));
    case CPXabs:
      CPX(Fetch(FetchWordPC()));

    case CPYimm:
      CPY(FetchPC());
    case CPYzpg:
      CPY(Fetch(FetchPC()));
    case CPYabs:
      CPY(Fetch(FetchWordPC()));

    case DECzpg:
      DEC(FetchPC());
    case DECabs:
      DEC(FetchWordPC());
    case DECzpgx:
      DEC(FetchPC()+x);
    case DECabsx:
      DEC(FetchWordPC()+x);

    case DEXimpl:
      DEX();

    case DEYimpl:
      DEY();

    case EORxind:
      EOR(Fetch(FetchWord(FetchWordPC()+x)));
    case EORzpg:
      EOR(Fetch(FetchPC()));
    case EORimm:
      EOR(FetchPC());
    case EORabs:
      EOR(Fetch(FetchWordPC()));
    case EORindy:
      EOR(Fetch(FetchWord(FetchWordPC())+y));
    case EORzpgx:
      EOR(Fetch(FetchPC()+x));
    case EORabsy:
      EOR(Fetch(FetchWordPC()+y));
    case EORabsx:
      EOR(Fetch(FetchWordPC()+x));

    case INCzpg:
      INC(FetchPC());
    case INCabs:
      INC(FetchWordPC());
    case INCzpgx:
      INC(FetchPC()+x);
    case INCabsx:
      INC(FetchWordPC()+x);

    case INXimpl:
      INX();

    case INYimpl:
      INY();

    case JMPabs:
      JMP(FetchWordPC());
    case JMPind:
      JMP(FetchWord(FetchWordPC()));

    case JSRabs:
      JSR(FetchWordPC());

    case LDAxind:
      LDA(Fetch(FetchWord(FetchWordPC()+x)));
    case LDAzpg:
      LDA(Fetch(FetchPC()));
    case LDAimm:
      LDA(FetchPC());
    case LDAabs:
      LDA(Fetch(FetchWordPC()));
    case LDAindy:
      LDA(Fetch(FetchWord(FetchWordPC())+y));
    case LDAzpgx:
      LDA(Fetch(FetchPC()+x));
    case LDAabsy:
      LDA(Fetch(FetchWordPC()+y));
    case LDAabsx:
      LDA(Fetch(FetchWordPC()+x));

    case LDXimm:
      LDX(FetchPC());
    case LDXzpg:
      LDX(Fetch(FetchPC()));
    case LDXabs:
      LDX(Fetch(FetchWordPC()));
    case LDXzpgy:
      LDX(Fetch(FetchPC()+y));
    case LDXabsy:
      LDX(Fetch(FetchWordPC()+y));

    case LDYimm:
      LDY(FetchPC());
    case LDYzpg:
      LDY(Fetch(FetchPC()));
    case LDYabs:
      LDY(Fetch(FetchWordPC()));
    case LDYzpgx:
      LDY(Fetch(FetchPC()+x));
    case LDYabsx:
      LDY(Fetch(FetchWordPC()+x));

    case LSRzpg:
      LSR(FetchPC());
    case LSRa:
      LSR();
    case LSRabs:
      LSR(FetchWordPC());
    case LSRzpgx:
      LSR(FetchPC()+x);
    case LSRabsx:
      LSR(FetchWordPC()+x);

    case NOPimpl:
      NOP();

    case ORAxind:
      ORA(Fetch(FetchWord(FetchWordPC()+x)));
    case ORAzpg:
      ORA(Fetch(FetchPC()));
    case ORAimm:
      ORA(FetchPC());
    case ORAabs:
      ORA(Fetch(FetchWordPC()));
    case ORAindy:
      ORA(Fetch(FetchWord(FetchWordPC())+y));
    case ORAzpgx:
      ORA(Fetch(FetchPC()+x));
    case ORAabsy:
      ORA(Fetch(FetchWordPC()+y));
    case ORAabsx:
      ORA(Fetch(FetchWordPC()+x));

    case PHAimpl:
      PHA();

    case PHPimpl:
      PHP();

    case PLAimpl:
      PLA();

    case PLPimpl:
      PLP();

    case ROLzpg:
      ROL(FetchPC());
    case ROLa:
      ROL();
    case ROLabs:
      ROL(FetchWordPC());
    case ROLzpgx:
      ROL(FetchPC()+x);
    case ROLabsx:
      ROL(FetchWordPC()+x);

    case RORzpg:
      ROR(FetchPC());
    case RORa:
      ROR();
    case RORabs:
      ROR(FetchWordPC());
    case RORzpgx:
      ROR(FetchPC()+x);
    case RORabsx:
      ROR(FetchWordPC()+x);

    case RTIimpl:
      RTI();

    case RTSimpl:
      RTS();

    case SBCxind:
      SBC(Fetch(FetchWord(FetchWordPC()+x)));
    case SBCzpg:
      SBC(Fetch(FetchPC()));
    case SBCimm:
      SBC(FetchPC());
    case SBCabs:
      SBC(Fetch(FetchWordPC()));
    case SBCindy:
      SBC(Fetch(FetchWord(FetchWordPC())+y));
    case SBCzpgx:
      SBC(Fetch(FetchPC()+x));
    case SBCabsy:
      SBC(Fetch(FetchWordPC()+y));
    case SBCabsx:
      SBC(Fetch(FetchWordPC()+x));

    case SECimpl:
      SEC();

    case SEDimpl:
      SED();

    case SEIimpl:
      SEI();

    case STAxind:
      STA(FetchWord(FetchWordPC()+x));
    case STAzpg:
      STA(FetchPC());
    case STAabs:
      STA(FetchWordPC());
    case STAindy:
      STA(FetchWord(FetchWordPC())+y);
    case STAzpgx:
      STA(FetchPC()+x);
    case STAabsy:
      STA(FetchWordPC()+y);
    case STAabsx:
      STA(FetchWordPC()+x);

    case STXzpg:
      STX(FetchPC());
    case STXabs:
      STX(FetchWordPC());
    case STXzpgy:
      STX(FetchPC()+y);

    case STYzpg:
      STY(FetchPC());
    case STYabs:
      STY(FetchWordPC());
    case STYzpgx:
      STY(FetchPC()+x);

    case TAXimpl:
      TAX();

    case TAYimpl:
      TAY();

    case TSXimpl:
      TSX();

    case TXAimpl:
      TXA();

    case TXSimpl:
      TXS();

    case TYAimpl:
      TYA();
  }
}
