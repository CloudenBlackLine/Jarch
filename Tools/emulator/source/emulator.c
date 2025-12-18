/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#include "../include/util.h"
#include "../include/emulator.h"

static void run_emu();

void
emulator(s32 argc, s8* argv[])
{
	struct termios old_termios;
	set_key(&old_termios);
	reg[SP] = 0, reg[PC] = 0, reg[LR] = 0;
	readfile(argv[1]);
	run_emu();
	reset_key(&old_termios);
	writefile(argv[1]);
	return;
}



static void
run_emu()
{
	s32 running = 1;
	while(running)
	{
		Instruction in = unpack(mem[reg[PC]++]);
		switch(in.opcode)
		{
			case OPCODE_MOV_IMM:		reg[in.r1] = in.imm; break;
			case OPCODE_MOV_REG: 		reg[in.r1] = reg[in.r2]; break;
			case OPCODE_MOV_REG_VAL:
			{
				reg[in.r1] = mem[reg[in.r2]/8];
				break;
			}
			case OPCODE_MOV_ADDR_VAL:
			{
				reg[in.r1] = mem[in.imm/8];
				break;
			}
			case OPCODE_MOV_LABEL_ADDR_VAL:
			{
				reg[in.r1] = mem[mem[in.imm/8]/8];
				break;
			}
			case OPCODE_REG_VAL_IMM:
			{
				if(check_const(reg[in.r1]))
				mem[reg[in.r1]/8] = in.imm;
				break;
			}
			case OPCODE_REG_VAL_REG:
			{
				mem[reg[in.r1]/8] = reg[in.r2];
				break;
			}
			case OPCODE_REG_VAL_REG_VAL:
			{
				mem[reg[in.r1]/8] = mem[reg[in.r2]/8];
				break;
			}
			case OPCODE_REG_VAL_ADDR_VAL:
			{
				mem[reg[in.r1]/8] = mem[in.imm/8];
				break;
			}
			case OPCODE_REG_VAL_LABEL_ADDR_VAL:
			{
				mem[reg[in.r1]/8] = mem[mem[in.imm/8]/8];
				break;
			}
			case OPCODE_LABEL_ADDR_VAL_REG:
			{
				mem[mem[in.imm/8]/8] = reg[in.r1];
				break;
			}
			case OPCODE_LABEL_ADDR_VAL_REG_VAL:
			{
				mem[mem[in.imm/8]/8] = mem[reg[in.r1]/8];
				break;
			}
			case OPCODE_ADDR_VAL_REG:
			{
				mem[in.imm/8] = reg[in.r1];
				break;
			}
			case OPCODE_ADDR_VAL_REG_VAL:
			{
				mem[in.imm/8] = mem[reg[in.r2]/8];
				break;
			}
			case OPCODE_PUSH_IMM:		mem[reg[SP]/8] = in.imm; reg[SP] = reg[SP] - 8; break;
			case OPCODE_PUSH_REG:		mem[reg[SP]/8] = reg[in.r1]; reg[SP] = reg[SP] - 8; break;
			case OPCODE_PUSH_REG_VAL:	mem[reg[SP]/8] = mem[reg[in.r1]/8]; reg[SP] = reg[SP] - 8; break;
			case OPCODE_PUSH_ADDR_VAL:	mem[reg[SP]/8] = mem[in.imm/8]; reg[SP] = reg[SP] - 8; break;
			case OPCODE_PUSH_LABEL_ADDR_VAL:mem[reg[SP]/8] = mem[mem[in.imm/8]/8]; reg[SP] = reg[SP] - 8; break;

			case OPCODE_POP_REG:		reg[SP] = reg[SP] + 8; reg[in.r1] = mem[reg[SP]/8]; break;
			case OPCODE_POP_REG_VAL:	reg[SP] = reg[SP] + 8; mem[reg[in.r1]/8] = mem[reg[SP]/8]; break;
			case OPCODE_POP_ADDR_VAL:	reg[SP] = reg[SP] + 8; mem[in.imm/8]= mem[reg[SP]/8]; break;
			case OPCODE_POP_LABEL_ADDR_VAL:	reg[SP] = reg[SP] + 8; mem[mem[in.imm/8]/8] = mem[reg[SP]/8]; break;

			case OPCODE_ADD_REG_IMM:	reg[in.r1] = reg[in.r2] + in.imm; break;
			case OPCODE_ADD_REG_REG:	reg[in.r1] = reg[in.r2] + reg[in.r3]; break;

			case OPCODE_SUB_REG_IMM:	reg[in.r1] = reg[in.r2] - in.imm; break;
			case OPCODE_SUB_IMM_REG:	reg[in.r1] = in.imm - reg[in.r3]; break;
			case OPCODE_SUB_REG_REG:	reg[in.r1] = reg[in.r2] - reg[in.r3]; break;

			case OPCODE_DIV_REG_IMM:	reg[in.r1] = reg[in.r2] / in.imm; break;
			case OPCODE_DIV_IMM_REG:	reg[in.r1] = in.imm / reg[in.r3]; break;
			case OPCODE_DIV_REG_REG:	reg[in.r1] = reg[in.r2] / reg[in.r3]; break;

			case OPCODE_MUL_REG_IMM:	reg[in.r1] = reg[in.r2] * in.imm; break;
			case OPCODE_MUL_REG_REG:	reg[in.r1] = reg[in.r2] * reg[in.r3]; break;

			case OPCODE_LSHIFT_REG_IMM:	reg[in.r1] = reg[in.r2] << in.imm; break;
			case OPCODE_LSHIFT_IMM_REG:	reg[in.r1] = in.imm << reg[in.r3]; break;
			case OPCODE_LSHIFT_REG_REG:	reg[in.r1] = reg[in.r2] << reg[in.r3]; break;

			case OPCODE_RSHIFT_REG_IMM:	reg[in.r1] = reg[in.r2] >> in.imm; break;
			case OPCODE_RSHIFT_IMM_REG:	reg[in.r1] = in.imm >> reg[in.r3]; break;
			case OPCODE_RSHIFT_REG_REG:	reg[in.r1] = reg[in.r2] >> reg[in.r3]; break;

			case OPCODE_OR_REG_IMM:		reg[in.r1] = reg[in.r2] | in.imm; break;
			case OPCODE_OR_IMM_REG:		reg[in.r1] = in.imm | reg[in.r3]; break;
			case OPCODE_OR_REG_REG:		reg[in.r1] = reg[in.r2] | reg[in.r3]; break;

			// case OPCODE_MASK_REG_IMM:	reg[in.r1] = reg[in.r2] & in.imm; break;
			// case OPCODE_MASK_IMM_REG:	reg[in.r1] = in.imm & reg[in.r3]; break;
			// case OPCODE_MASK_REG_REG:	reg[in.r1] = reg[in.r2] & reg[in.r3]; break;

			case OPCODE_CMP_EQ_ADDR:	if(reg[in.r1] == reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;
			case OPCODE_CMP_NE_ADDR:	if(reg[in.r1] != reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;
			case OPCODE_CMP_GT_ADDR:	if(reg[in.r1] < reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;
			case OPCODE_CMP_LT_ADDR:	if(reg[in.r1] > reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;

			case OPCODE_JMP_ADDR:		reg[LR] = reg[PC]*0x8; reg[PC] = in.imm/8; break;
			case OPCODE_JMP_REG:		reg[LR] = reg[PC]*0x8; reg[PC] = reg[in.r1]/8; break;
			case OPCODE_RET:			reg[PC] = reg[LR]/0x8; break;
			case OPCODE_HALT: running = 0; break;
		}
		// mem[SYS_TIMER_BASE_ADDR / 8] = ++system_timer;
	}
}

bool
check_const(u64 addr)
{
	return (addr > (RAM_CONST_BASE+RAM_CONST_SIZE)
			|| addr < RAM_CONST_BASE);
}
