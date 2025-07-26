#pragma once
#include <cstdint>
#define byte uint8_t

typedef union {
  byte header[16];
  struct {
    byte NES[4];
    byte prgromunits;
    byte chrromunits;
    struct {
      byte is_vertical_mirror : 1;
      byte contains_battery : 1;
      byte uses_trainer : 1;
      byte alt_nametable_layout : 1;
      byte mapper_lo_nybble : 4;
    };
    struct {
      byte vs_unisystem : 1;
      byte playchoice10 : 1;
      byte inesversion : 2;
      byte mapper_hi_nybble : 4;
    };
    byte prgramunits;
    byte tv_system_unused;
    struct {
      byte tv_system : 2;
      byte : 2;
      byte has_prgram : 1;
      byte has_bus_conflicts : 1;
    };
  };
} iNES_v1_header;

typedef union {
  byte header[16];
  struct {
    byte NES[4];
    byte prgromunits;
    byte chrromunits;
    struct {
      byte is_vertical_mirror : 1;
      byte contains_battery : 1;
      byte uses_trainer : 1;
      byte alt_nametable_layout : 1;
      byte mapper_lo_nybble : 4;
    };
    struct {
      byte vs_unisystem : 1;
      byte playchoice10 : 1;
      byte inesversion : 2;
      byte mapper_hi_nybble : 4;
    };
    byte prgramunits;
    byte tv_system_unused;
    struct {
      byte tv_system : 2;
      byte : 2;
      byte has_prgram : 1;
      byte has_bus_conflicts : 1;
    };
  };
} iNES_v2_header; // TODO: Update with correct format

typedef union {
  iNES_v1_header v1;
  iNES_v2_header v2;
} iNES_header;
