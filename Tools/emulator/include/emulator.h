/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#ifndef EMULATOR_H
#define EMULATOR_H

#include "../../include/jarch.h"
#include "../../include/types.h"


extern u64          mem             [JARCH_MEMORY_SIZE];

extern u64          system_timer;

extern u64          reg             [MAX_REGISTER];
extern u64          SP;
extern u64          PC;
extern u64          LR;


void        emulator(s32,s8**);

bool        check_const(u64);

#endif
