#include "./header.h"

#define		MAX_MEMORY	0x00007d00
#define		MAX_REGISTER	13

void		readfile(s8* filename);
void		writefile(s8* filename);

void		set_key(struct termios* original_termios);
void		reset_key(struct termios* old_termios);

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
	readfile(argv[1]);
	emulator();
	writefile(argv[1]);
	reset_key(&old_termios);
	return 0;
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

			case OPCODE_CMP_EQ_ADDR:	if(reg[in.r1] == reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;
			case OPCODE_CMP_NE_ADDR:	if(reg[in.r1] != reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;
			case OPCODE_CMP_GT_ADDR:	if(reg[in.r1] < reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;
			case OPCODE_CMP_LT_ADDR:	if(reg[in.r1] > reg[in.r2]) { reg[LR] = reg[PC]; reg[PC] = in.imm / 8; } break;

			case OPCODE_JMP_ADDR:		reg[LR] = reg[PC]; reg[PC] = in.imm/8; break;
			case OPCODE_JMP_REG:		reg[LR] = reg[PC]; reg[PC] = reg[in.r1]; break;
			case OPCODE_RET:		reg[PC] = reg[LR]; break;
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
	size_t index_ = 0;
	u64 value = 0;
	s32 count = 0;
	s32 c;
	while((c = fgetc(file)) != EOF)
	{
		if(c == ';')
		{
			if(index_ >= MAX_MEMORY / 8) break;
			mem[index_] = value;
			index_++;
			value = 0;
			count = 0;
			c = fgetc(file);
			if(c != '\n') return;
		}
		else if(c == '\n' || c == ' ' || c == '\t' || c == '\r')
		continue;
		else
		{
			value <<= 4;
			if(c >= '0' && c <= '9') value |= (c - '0');
			else if(c >= 'a' && c <= 'f') value |= (c - 'a'+10);
			else { fprintf(stderr, "Invalid hex char\n"); exit(1); }
			count++;
		}
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
	FILE* f = fopen(filename, "w");
	if(!f) return;
	for(u32 i=0; i<MAX_MEMORY/8; i++)
	{
		{
			Instruction a = unpack(mem[i]);
			fprintf(f, "%02x%02x%02x%02x%08x;\n", a.opcode, a.r1, a.r2, a.r3, a.imm);
		}
	}
	fclose(f);
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