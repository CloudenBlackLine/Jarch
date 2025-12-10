/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#ifndef     ASSEMBLER_H
#define     ASSEMBLER_H

#include "../../include/types.h"
#include "../../include/jarch.h"

typedef struct __attribute__((packed))
{
	u8 		opcode;
	u8 		r1;
	u8 		r2;
	u8 		r3;
	u32 	imm;
} Instruction;



extern s8			acode			[MAX_ASM_SYNTAX_BUFFER];
extern u64			asm_syntax_size;

extern u64			text_buffer		[MAX_TEXT_BUFFER];
extern u64			text_offset;

extern u64			data_buffer		[MAX_DATA_BUFFER];
extern u64			data_offset;

extern u64			const_buffer	[MAX_CONST_BUFFER];
extern u64			const_offset;

extern Relocate		relocate_buffer	[MAX_ASM_LABEL_TABLE/2];
extern u64			relocate_offset;


extern Symbol 		labels			[MAX_ASM_LABEL_TABLE];
extern u64			symbol_count;




void	    assembler               ();

void        first_pass              (s8* c, u64 l1);
void        second_pass             (s8* c, u64 l1);
s8*		    section_const_line      (s8* code);
s8*		    section_bss_line        (s8* code);
s8*		    section_data_line       (s8* code);
s8*		    section_text_line       (s8* code);


#endif
