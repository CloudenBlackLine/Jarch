/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#ifndef UTIL_H
#define UTIL_H

#include "../../include/jarch.h"
#include "../../include/types.h"
#include "./emulator.h"

#include "../../asm/include/assembler.h"

void		readfile      (s8*);
void		writefile     (s8*);

Instruction	unpack        (u64 val);

void		set_key       (struct termios*);
void		reset_key     (struct termios*);

#endif
