#include "./header.h"

#define 	MAX_MEMORY	0x00007d00

void		readfile(s8* filename);
void		debuger();
void		filewrite();

Instruction	unpack(u64 val);

u64		mem[MAX_MEMORY/8];
u64		reg[9];
u32		index_list[MAX_MEMORY/8];
u64		pc = 0;

s8*		filename;

s32
main(s32 argc, s8* argv[])
{
	if(argc < 2) return 1;
	readfile(argv[1]);
	filewrite();
	return 0;
}

void
readfile(s8* f)
{
	filename = f;
	FILE* file = fopen(f, "r");
	if(!file) return;
	size_t index_ = 0;
	size_t addr_index = 0x0;
	u64 i = 0;
	u64 value = 0;
	s32 count = 0;
	s32 c;
	while((c = fgetc(file)) != EOF)
	{
		if(c == ';')
		{
			if(index_ >= MAX_MEMORY / 8) break;
			if(value != 0x0000000000000000) { mem[index_] = value; index_list[index_++] = addr_index; i++; }
			addr_index+=8;
			value = 0, count = 0;
			c = fgetc(file);
			if(c != '\n') return;
		}
		else if(c == '\n' || c == ' ' || c == '\t' || c == '\r') continue;
		else
		{
			value <<= 4;
			if(c >= '0' && c <= '9') value |= (c - '0');
			else if(c >= 'a' && c <= 'f') value |= (c - 'a'+10);
			else { fprintf(stderr, "Invalid hex char\n"); exit(1); }
			count++;
		}
	}
	pc = i;
	fclose(file);
}


void
filewrite()
{
	int i = 0;
	s8 buffer[32];
	while(*filename!='\0' && *filename != '.')
	buffer[i++] = *filename++;
	buffer[i++] = '.';
	buffer[i++] = 'j';
	buffer[i++] = 'b';
	buffer[i++] = 'u';
	buffer[i++] = 'g';
	buffer[i] = '\0';
	FILE* f = fopen(buffer, "w");
	if(!f) return;
	fprintf(f, "[   ADDR   ] |opcode| reg1 | reg2 | reg3 |   imm    |\n");
	for(u32 i=0; i<pc; i++)
	{
		{
			Instruction a = unpack(mem[i]);
			fprintf(f, "[ %08x ] |  %02x  |  %02x  |  %02x  |  %02x  | %08x |\n", index_list[i], a.opcode, a.r1, a.r2, a.r3, a.imm);
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