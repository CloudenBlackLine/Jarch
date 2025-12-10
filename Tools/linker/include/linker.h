/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#ifndef LINKER_H
#define LINKER_H


// Includes

#include "../../include/jarch.h"
#include "../../include/types.h"



// Types


typedef struct
{
	u64	text_			[ROM_TEXT_SIZE];
	u64	t_count;
	u64	const_			[RAM_CONST_SIZE];
	u64 	c_count;
	u64	data_			[RAM_DATA_SIZE];
	u64 	d_count;
	u32	index;
	s8	name[64];
} File;


typedef struct
{
	u64 addr;
	u64 value;
} Opcode;


// External

extern Symbol	symbol_buffer		[MAX_ASM_LABEL_TABLE*4];
extern u64		symbol_count;

extern Opcode*	hex_buffer;
extern u64		hexb_count;

extern File*	file;
extern u32		file_count;

extern Relocate	relocate			[MAX_ASM_LABEL_TABLE/2];
extern u64		relocate_count;



// Functions

void			linker				(s32,s8**);

s8*				read_labels			(s8*, u32);
s8*				read_raw_hex		(s8*);
void			read_relocation		(s8*);

void			write_text			();
void			write_data			();
void			write_const			();
void			do_labels			();
void			do_relocate			();

#endif
