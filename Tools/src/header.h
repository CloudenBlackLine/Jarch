#ifndef HEADER
#define HEADER

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef char		s8;
typedef short		s16;
typedef int		s32;
typedef long		s64;

typedef enum { SECTION_NO, SECTION_INIT, SECTION_DATA, SECTION_TEXT, SECTION_UNIT } Section;
typedef enum { FLAG_COMPLETE, FLAG_LABEL, FLAG_ADDR, FLAG_VAR, FLAG_VAR_LIST } Flag;

typedef struct __attribute__((packed))
{
	u8 	opcode;
	u8 	r1;
	u8 	r2;
	u8 	r3;
	u32 	imm;
} Instruction;

typedef struct __attribute__((packed))
{
	s8		name[32];
	u64		addr;
	u64		offset;
} UnitMade;

typedef struct __attribute__((packed))
{
	s8		unit_name[32];
	s8		var_name[16][32];
	u64		var_count;
	u64		obj_count;
	u64		offset;
	UnitMade	obj[16];
} Unit;

typedef struct __attribute__((packed))
{
	s8 		name[32];
	u64		addr;
	Flag		flag;
	u64		opcode[16];
	u64		location[16];
	u64		count;
} Label;

typedef struct __attribute__((packed))
{
	u64	entry_point;
	s8	entry_label[32];
	s32	entry_flag;
	u64	max_labels;
	u64	max_memory;

	u64	data_base_addr;
	u64	text_base_addr;
	u64	unit_base_addr;
	u64	stack_base_addr;

	u64	u8_base_addr;
	u64	u16_base_addr;
	u64	u32_base_addr;
	u64	u64_base_addr;
} SystemConfig;

enum
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

	OPCODE_PUSH_IMM,
	OPCODE_PUSH_REG,
	OPCODE_PUSH_REG_VAL,
	OPCODE_PUSH_ADDR_VAL,
	OPCODE_PUSH_LABEL_ADDR_VAL,

	OPCODE_POP_REG,
	OPCODE_POP_REG_VAL,
	OPCODE_POP_ADDR_VAL,
	OPCODE_POP_LABEL_ADDR_VAL,

	OPCODE_LSHIFT_REG_IMM,
	OPCODE_LSHIFT_IMM_REG,
	OPCODE_LSHIFT_REG_REG,

	OPCODE_RSHIFT_REG_IMM,
	OPCODE_RSHIFT_IMM_REG,
	OPCODE_RSHIFT_REG_REG,

	OPCODE_OR_REG_IMM,
	OPCODE_OR_IMM_REG,
	OPCODE_OR_REG_REG,

	OPCODE_AND_IMM,
	OPCODE_AND_REG,

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
};


enum
{
	I_BASE_ADDR = 0x7000,
	O_BASE_ADDR = 0x7008,

	SENSOR_I_BASE_ADDR_0 = 0x7010,
	SENSOR_I_BASE_ADDR_1 = 0x7018,
	SENSOR_I_BASE_ADDR_2 = 0x7020,
	SENSOR_I_BASE_ADDR_3 = 0x7028,

	SYS_TIMER_BASE_ADDR = 0x7030,

	IO_FLAG_BASE = 0x7c00,
};

#endif