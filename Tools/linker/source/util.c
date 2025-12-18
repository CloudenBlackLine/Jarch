/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#include "../include/util.h"
#include "../include/linker.h"


void
writefile(const s8* file)
{
	FILE* f = fopen(file, "wb");
	if(!f) return;
	for(u32 i=0; i<hexb_count; i++)
	fprintf(f, "%016lx:%016lx;\n", hex_buffer[i].addr, hex_buffer[i].value);
	fclose(f);
}

s8*
readfile(const s8* filename)
{
	s32 fd = open(filename, O_RDONLY);
	u64 size;
	struct stat st;
	fstat(fd, &st);
	size = st.st_size;
	s8* loc = malloc(size);
	read(fd, loc, size);
	return loc;
}

bool
start_contains(s8* src, const s8* look)
{
	while(*look!='\0')
	{
		if(*look!=*src) return false;
		look++, src++;
	}
	return true;
}

s8*
read_line(s8* src, s8* dest)
{
	if(*src == '<') src++;
	while(*src && *src != '>')
	*dest++ = *src++;
	if(*src == '>') src+=2;
	*dest = 0;
	return src;
}


s8*
read_splice(s8* src, s8* dest)
{
	while(*src && *src != ',' && *src != '\n')
	*dest++ = *src++;
	if(*src == ',' || *src == '\n') *src++;
	*dest = 0;
	return src;
}


u64
read_hex(s8** p)
{
	u64 value = 0;
	while ( (**p >= '0' && **p <= '9') || (**p >= 'A' && **p <= 'F') || (**p >= 'a' && **p <= 'f') )
	{
		value <<= 4;
		if(**p >= '0' && **p <= '9')		value |= **p - '0';
		else if(**p >= 'A' && **p <= 'F')	value |= 10 + (**p - 'A');
		else if(**p >= 'a' && **p <= 'f')	value |= 10 + (**p - 'a');
		(*p)++;
	}
	return value;
}


u64
get_symbol_index(s8* name)
{
	for(u64 i=0; i<symbol_count; i++)
	if(!strcmp(symbol_buffer[i].name, name))
	return i;
	return UN_DEFINED_LABEL;
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


u64
packed(u8 opcode, u8 s1, u8 s2, u8 s3, u32 imm)
{
	u64 val = 0x0000000000000000;
	val |= ((u64)opcode << 56);
	val |= ((u64)s1 << 48);
	val |= ((u64)s2 << 40);
	val |= ((u64)s3 << 32);
	val |= ((u64)imm << 0);
	return val;
}

