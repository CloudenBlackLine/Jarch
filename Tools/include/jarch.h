/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#ifndef JARCH_H
#define JARCH_H

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include "./types.h"
#include <sys/stat.h>


#define	MAX_ASM_INSTRUCTION		0x00005203
#define MAX_ASM_LINE_LENGTH		50
#define MAX_ASM_EXTRA_SPACE		1000
#define	MAX_ASM_SYNTAX_BUFFER	((MAX_ASM_INSTRUCTION * MAX_ASM_LINE_LENGTH) + MAX_ASM_EXTRA_SPACE)
#define MAX_ASM_LABEL_TABLE		(MAX_ASM_INSTRUCTION / 0x2)

#define MAX_REGISTER	13

// Memory Layout

#define JARCH_MEMORY_SIZE       0x00100000

#define ROM_TEXT_BASE           0x00010000
#define ROM_TEXT_SIZE           0x00060000

#define RAM_DATA_BASE           0x00070000
#define RAM_DATA_SIZE           0x00020000

#define RAM_BSS_BASE            0x00090000
#define RAM_BSS_SIZE            0x00020000

#define RAM_CONST_BASE          0x000b0000
#define RAM_CONST_SIZE			0x0000f000

#define MMIO_BASE               0x000c0000



// Assembler -> object Layout

#define MAX_TEXT_BUFFER			(ROM_TEXT_SIZE/0x8)
#define MAX_DATA_BUFFER			(RAM_DATA_SIZE/0x8)
#define MAX_CONST_BUFFER		(RAM_CONST_SIZE/0x8)
#define MAX_BSS_BUFFER			(RAM_BSS_BASE/0x8)


#define UN_DEFINED_LABEL		(0xffffffff)



typedef enum { SECTION_NO=0, SECTION_DATA, SECTION_TEXT, SECTION_BSS, SECTION_CONST, SECTION_SCOPE } Section;
typedef enum { EXTERNAL=0, INTERNAL } Global_State;



// Linker




// Types

typedef struct
{
    s8              name[64];
    Section         section;
    u64             base_addr;
    u64             offset;
    bool            global;
    Global_State    global_state;
    s8		    filename[64];
} Symbol;


typedef struct
{
	s8	name[64];
	Section	section;
	u64	offset;
} Relocate;


typedef enum
{
	OPCODE_MOV_IMM = 0x01,
	OPCODE_MOV_REG,
	OPCODE_MOV_REG_VAL,
	OPCODE_MOV_ADDR_VAL,
	OPCODE_MOV_LABEL_ADDR_VAL,

	
	
	OPCODE_REG_VAL_IMM,
	OPCODE_REG_VAL_REG,
	OPCODE_REG_VAL_REG_VAL,
	OPCODE_REG_VAL_ADDR_VAL,
	OPCODE_REG_VAL_LABEL_ADDR_VAL,

	
	
	OPCODE_ADDR_VAL_REG,
	OPCODE_ADDR_VAL_REG_VAL,
	
	
	
	OPCODE_LABEL_ADDR_VAL_REG,
	OPCODE_LABEL_ADDR_VAL_REG_VAL,

	
	
	OPCODE_PUSH_IMM,
	OPCODE_PUSH_REG,
	OPCODE_PUSH_REG_VAL,
	OPCODE_PUSH_ADDR_VAL,
	OPCODE_PUSH_LABEL_ADDR_VAL,

	OPCODE_POP_REG,
	OPCODE_POP_REG_VAL,
	OPCODE_POP_ADDR_VAL,
	OPCODE_POP_LABEL_ADDR_VAL,


	
	
	OPCODE_ADD_REG_IMM,
	OPCODE_ADD_REG_REG,

	OPCODE_SUB_REG_IMM,
	OPCODE_SUB_IMM_REG,
	OPCODE_SUB_REG_REG,

	OPCODE_MUL_REG_IMM,
	OPCODE_MUL_REG_REG,

	OPCODE_DIV_REG_IMM,
	OPCODE_DIV_IMM_REG,
	OPCODE_DIV_REG_REG,

	

	OPCODE_LSHIFT_REG_IMM,
	OPCODE_LSHIFT_IMM_REG,
	OPCODE_LSHIFT_REG_REG,

	OPCODE_RSHIFT_REG_IMM,
	OPCODE_RSHIFT_IMM_REG,
	OPCODE_RSHIFT_REG_REG,

	OPCODE_OR_REG_IMM,
	OPCODE_OR_IMM_REG,
	OPCODE_OR_REG_REG,

	OPCODE_XOR_REG_IMM,
	OPCODE_XOR_REG_REG,

	OPCODE_AND_REG_IMM,
	OPCODE_AND_IMM_REG,
	OPCODE_AND_REG_REG,

	OPCODE_NOT_REG,


	
	OPCODE_CMP_EQ_ADDR,
	OPCODE_CMP_NE_ADDR,
	OPCODE_CMP_LT_ADDR,
	OPCODE_CMP_GT_ADDR,

	OPCODE_CMP_EQ_REG,
	OPCODE_CMP_NE_REG,
	OPCODE_CMP_LT_REG,
	OPCODE_CMP_GT_REG,

	OPCODE_CMP_IMM_EQ_REG,
	OPCODE_CMP_IMM_NE_REG,
	OPCODE_CMP_IMM_LT_REG,
	OPCODE_CMP_IMM_GT_REG,

	OPCODE_CMP_GTE_ADDR,
	OPCODE_CMP_LTE_ADDR,

	OPCODE_CMP_GTE_REG,
	OPCODE_CMP_LTE_REG,

	OPCODE_CMP_IMM_GTE_REG,
	OPCODE_CMP_IMM_LTE_REG,

	
	
	OPCODE_JMP_ADDR,
	OPCODE_JMP_REG,

	
	
	OPCODE_HALT,
	OPCODE_RET,
} OPCODES_;



typedef struct __attribute__((packed))
{
	u64		entry_point;
	s8		entry_label[32];
	s32		entry_flag;
	u64		max_labels;
	u64		max_memory;
	u64		data_base_addr_rom;
	u64		data_base_addr_ram;
	u64		text_base_addr_rom;
	u64		text_base_addr_ram;
} SystemConfig;



#endif
