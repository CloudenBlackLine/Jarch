/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#ifndef UTIL_H
#define UTIL_H

#include "./assembler.h"
#include "../../include/types.h"
#include "../../include/jarch.h"



s32 	    readfile                (s8*,s8*,u32);
void        readbuffer              (s8*,u32,s8*);
void	    writefile               (s8*);

void	    skip_ws                 (s8*,s8*);

u64		    get_hex                 (s8**);
u8		    get_reg                 (s8**);
s8*		    get_name                (s8*,s8*);
s8*         get_label               (s8*,s8*,Section);
s8*         get_line                (s8*,s8*);

bool        start_contains          (s8*,const s8*);

u64		    packed                  (u8,u8,u8,u8,u32);

OPCODES_    math1_op_ret            (s8,s8);
OPCODES_    math2_op_ret            (s8,s8,s8);
u64         calculate_offset        (s8**);

void        add_relocate            (s8*,Section,u64,u64);

#endif
