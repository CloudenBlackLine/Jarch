#include "./header.h"

#define		MAX_MEMORY	0x00007ff8
#define		MAX_REGISTER	13

void		readfile(s8* filename);
void		writefile(s8* filename);

void		set_key(struct termios* original_termios);
void		reset_key(struct termios* old_termios);


void		init_mem();
void		emulator();

Instruction	unpack(u64 val);


u64		mem[MAX_MEMORY/8];
u64		system_timer = 0;

u64		reg[MAX_REGISTER];
s32		SP = 0x0a;
s32		PC = 0x0b;
s32		LR = 0x0c;

s32
main(s32 argc, s8* argv[])
{
	struct termios old_termios;
	set_key(&old_termios);
	reg[SP] = 0, reg[PC] = 0, reg[LR] = 0;
	init_mem();
	readfile(argv[1]);
	emulator();
	writefile(argv[2]);
	reset_key(&old_termios);
	return 0;
}

void
init_mem()
{
	for(int i=0; i<MAX_MEMORY-1/8; i++)
	mem[i] = 0;
}


void
emulator()
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
				if(reg[in.r2] == I_BASE_ADDR) mem[reg[in.r2]/8] = (u8)getchar();
				reg[in.r1] = mem[reg[in.r2]/8];
				break;
			}
			case OPCODE_MOV_ADDR_VAL:
			{
				if(in.imm == I_BASE_ADDR) mem[in.imm/8] = (u8)getchar();
				reg[in.r1] = mem[in.imm/8];
				break;
			}
			case OPCODE_MOV_LABEL_ADDR_VAL:
			{
				if(mem[in.imm/0x8] == I_BASE_ADDR) mem[mem[in.imm/8]/8] = (u8)getchar();
				reg[in.r1] = mem[mem[in.imm/8]/8];
				break;
			}
			case OPCODE_REG_VAL_IMM:
			{
				if(reg[in.r1] == O_BASE_ADDR) putchar((s8)in.imm);
				mem[reg[in.r1]/8] = in.imm;
				break;
			}
			case OPCODE_REG_VAL_REG:
			{
				if(reg[in.r1] == O_BASE_ADDR) putchar((s8)reg[in.r2]);
				mem[reg[in.r1]/8] = reg[in.r2];
				break;
			}
			case OPCODE_REG_VAL_REG_VAL:
			{
				if(reg[in.r2] == I_BASE_ADDR) mem[reg[in.r2]/8] = (u8)getchar();
				if(reg[in.r1] == O_BASE_ADDR) putchar((s8)mem[reg[in.r2]/8]);
				mem[reg[in.r1]/8] = mem[reg[in.r2]/8];
				break;
			}
			case OPCODE_REG_VAL_ADDR_VAL:
			{
				if(reg[in.r2] == I_BASE_ADDR) mem[in.imm/8] = (u8)getchar();
				if(reg[in.r1] == O_BASE_ADDR) putchar((s8)mem[in.imm/8]);
				mem[reg[in.r1]/8] = mem[in.imm/8];
				break;
			}
			case OPCODE_REG_VAL_LABEL_ADDR_VAL:
			{
				if(mem[in.imm/0x8] == I_BASE_ADDR) mem[reg[in.r1]/8] = (u8)getchar();
				if(reg[in.r1]/0x8 == O_BASE_ADDR) putchar((s8)mem[mem[in.imm/8]/8]);
				mem[reg[in.r1]/8] = mem[mem[in.imm/8]/8];
				break;
			}
			case OPCODE_LABEL_ADDR_VAL_REG:
			{
				if(mem[in.imm/0x8] == O_BASE_ADDR) putchar((s8)reg[in.r1]);
				mem[mem[in.imm/8]/8] = reg[in.r1];
				break;
			}
			case OPCODE_LABEL_ADDR_VAL_REG_VAL:
			{
				if(reg[in.r1]/0x8 == I_BASE_ADDR) mem[mem[in.imm/8]/8] = (u8)getchar();
				if(mem[in.imm/0x8] == O_BASE_ADDR) putchar((s8)mem[reg[in.r1]/8]);
				mem[mem[in.imm/8]/8] = mem[reg[in.r1]/8];
				break;
			}
			case OPCODE_ADDR_VAL_REG:
			{
				if(in.imm == O_BASE_ADDR) putchar((s8)reg[in.r1]);
				mem[in.imm/8] = reg[in.r1];
				break;
			}
			case OPCODE_ADDR_VAL_REG_VAL:
			{
				if(reg[in.r2] == I_BASE_ADDR) mem[reg[in.r2]/8] = (u8)getchar();
				if(in.imm == O_BASE_ADDR) putchar((s8)mem[reg[in.r2]/8]);
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

			case OPCODE_MASK_REG_IMM:	reg[in.r1] = reg[in.r2] & in.imm; break;
			case OPCODE_MASK_IMM_REG:	reg[in.r1] = in.imm & reg[in.r3]; break;
			case OPCODE_MASK_REG_REG:	reg[in.r1] = reg[in.r2] & reg[in.r3]; break;

			case OPCODE_CMP_EQ_ADDR:	if(reg[in.r1] == reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;
			case OPCODE_CMP_NE_ADDR:	if(reg[in.r1] != reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;
			case OPCODE_CMP_GT_ADDR:	if(reg[in.r1] < reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;
			case OPCODE_CMP_LT_ADDR:	if(reg[in.r1] > reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;

			case OPCODE_JMP_ADDR:		reg[LR] = reg[PC]*0x8; reg[PC] = in.imm/8; break;
			case OPCODE_JMP_REG:		reg[LR] = reg[PC]*0x8; reg[PC] = reg[in.r1]/8; break;
			case OPCODE_RET:			reg[PC] = reg[LR]/0x8; break;
			case OPCODE_HALT: running = 0; break;
		}
		mem[SYS_TIMER_BASE_ADDR / 8] = ++system_timer;
	}
}

void
readfile(s8* f)
{
	FILE* file = fopen(f, "r");
	if(!file) return;
	s32 c;
	u32 count = 0;
	u64 addr = 0;
	s8 buffer[16];
	while((c = fgetc(file)) != EOF)
	{
		if(c == ':')
		{
			count = 0;
			addr = strtoull(buffer, NULL, 16);
			continue;
		}
		else if(c == ';')
		{
			count = 0;
			mem[addr] = strtoull(buffer, NULL, 16);
			addr = 0;
			continue;
		}
		else if(c == ' ' || c == '\n' || c == '\r') continue;
		buffer[count++] = (char)c;
	}
	fclose(file);
}

void
set_key(struct termios* old_termios)
{
	struct termios new_termios;
	tcgetattr(STDIN_FILENO, old_termios);
	new_termios = *old_termios;
	new_termios.c_lflag &= ~(ICANON | ECHO);
	new_termios.c_cc[VMIN] = 1;
	new_termios.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void
reset_key(struct termios* old_termios)
{
	tcsetattr(STDIN_FILENO, TCSANOW, old_termios);
}

void
writefile(s8* filename)
{
	
}

Instruction
unpack(u64 val)
{
	Instruction a;
	a.opcode = (val >> 56) & 0xff;
	a.r1 = (val >> 48) & 0xff;
	a.r2 = (val >> 40) & 0xff;
	a.r3 = (val >> 32) & 0xff;
	a.imm = (val >> 0) & 0xffffffff;
	return a;
}

