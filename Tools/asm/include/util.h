#ifndef UTIL_H
#define UTIL_H

#include "./assembler.h"
#include "../../include/types.h"
#include "../../include/jarch.h"



s32 	    readfile                (s8* filename, s8* loc, u32 size);
void        readbuffer              (s8* loc, u32 size, s8* buffer);
void	    writefile               (s8* filename);

void	    skip_ws                 (s8* dest, s8* src);

u64		    get_hex                 (s8** p);
u8		    get_reg                 (s8** p);
s8*		    get_name                (s8* dest, s8* src);

s8*         get_label               (s8* dest, s8* src, Section s);
s8*         get_line                (s8* dest, s8* src);

u64         find_label              (s8* name);
u64         find_addr_value         (u64 addr, u64* loc, u64 base);

bool        start_contains          (s8* s1, const s8* s2);

u64		    packed                  (u8 r1, u8 r2, u8 r3, u8 r4, u32 imm);
Instruction	unpack                  (u64 value);

#endif

OPCODES_    math1_op_ret             (s8 t, s8 flag);
OPCODES_    math2_op_ret             (s8 t, s8 flag1, s8 flag2);
