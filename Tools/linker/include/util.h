/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#ifndef UTIL_H
#define UTIL_H


#include "../../include/types.h"
#include "../../include/jarch.h"
#include "../../asm/include/assembler.h"


void		writefile	    (const s8*);
s8* 		readfile	    (const s8*);

bool		start_contains	(s8*, const s8*);
s8*		    read_line       (s8*,s8*);
s8*		    read_splice     (s8*,s8*);
u64		    read_hex        (s8**);

u64         get_symbol_index(s8*);

Instruction unpack          (u64);
u64         packed          (u8,u8,u8,u8,u32);


#endif
