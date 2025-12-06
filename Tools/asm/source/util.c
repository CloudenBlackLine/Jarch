#include "../include/util.h"



s32
readfile(s8* filename, s8* loc, u32 size)
{
	s32 fd = open(filename, O_RDONLY);
	if(fd < 0) return -2;
	s32 s = read(fd, loc, size);
	if(s < 0) { close(fd); return -1; }
	close(fd);
	loc[s] = 0;
	return s;
}

void
readbuffer(s8* loc, u32 size, s8* buffer)
{
	s32 i = 0;
	while(i < size || *buffer != '\0')
	loc[i++] = *buffer++;
}


void
writefile(s8* filename)
{
	FILE* f = fopen(filename, "w");
	if(!f) return;

	fprintf(f, "(label)\n");
	for(u32 i=0; i<symbol_count; i++)
	{
		fprintf(f, "<%s,%01x,%016lx,%08lx,%u,%u>\n", 
		labels[i].name, (u32)labels[i].section, labels[i].base_addr, labels[i].offset, labels[i].global, (bool)labels[i].global_state);
	}
	fprintf(f, "(text)\n");
	for(u64 i=0; i<text_offset; i++)
	fprintf(f, "<%016lx>\n", text_buffer[i]);

	fprintf(f, "(data)\n");
	for(u64 i=0; i<data_offset; i++)
	fprintf(f, "<%016lx>\n", data_buffer[i]);


	fprintf(f, "(const)\n");
	for(u64 i=0; i<const_offset; i++)
	fprintf(f, "<%016lx>\n", const_buffer[i]);

	fprintf(f, "(relocate)\n");
	for(u64 i=0; i<relocate_offset; i++)
	fprintf(f, "<%s,%01x,%08lx>\n", relocate_buffer[i].name, relocate_buffer[i].section, relocate_buffer[i].offset);

	fclose(f);
}
 
void
skip_ws(s8* dest, s8* src)
{
	u32 i = 0, j = 0;
    bool in_comment = false;

    while (src[i])
    {
        s8 c = src[i];
        if (in_comment)
		{
            if (c == '>') in_comment = false;
            i++;
            continue;
        }
		if (c == '<')
		{
            in_comment = true, i++;
            continue;
        }

        if (c!='{' && c!='}' && c!=' ' && c!='\r' && c!='\n' && c!='\t')
        dest[j++] = c;
        i++;
    }

    dest[j] = 0;
}

u64
get_hex(s8** p)
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

u8
get_reg(s8** p)
{
	s8* str = *p;
	u8 value = 0xff;
	if(*str >= '0' && *str <= '9') { value = (u8)(*str - '0'); *p = str + 1; return value; }
	if(str[0] == 's' && str[1] == 'p') { *p = str + 2; return 0x0a; }
	if(str[0] == 'p' && str[1] == 'c') { *p = str + 2; return 0x0b; }
	if(str[0] == 'l' && str[1] == 'r') { *p = str + 2; return 0x0c; }
	return value;
}

s8*
get_name(s8* dest, s8* src)
{
	while(*src && *src != ':' && *src != '$' && *src != ';' && *src != '=' && *src != ',' && *src != '[' && *src != '{' && *src != '.' && *src != '(' && *src != '#' && *src != '*')
	*dest++ = *src++;
	*dest = 0;
	return src;
}

s8*
get_label(s8* dest, s8* src, Section sec)
{
	switch(sec)
	{
		case SECTION_NO:
			while (*src && *src != ';' && *src != ':' && *src != '.') *dest++ = *src++;
			break;
		case SECTION_TEXT:
    		while (*src && *src != ':' && *src != ';') *dest++ = *src++;
			break;
		case SECTION_CONST:
		case SECTION_DATA:
			while (*src && *src != ';' && *src != '[' && *src != ',' && *src != '=') *dest++ = *src++;
			break;
		case SECTION_BSS:
			while (*src && *src != ';' && *src != '[') *dest++ = *src++;
			break;
	}
    *dest = 0;
    return src; 
}

s8*
get_line(s8* dest, s8* src)
{
	while (*src && *src != ';') *dest++ = *src++;
	*dest = 0;
	return src;
}

u64
find_label(s8* name)
{
	for(u64 i=0; i<symbol_count; i++)
	if(!strcmp(labels[i].name, name)) return labels[i].base_addr;
	return UN_DEFINED_LABEL;
}

u64
find_addr_value(u64 addr, u64* loc, u64 base)
{
	return loc[addr-base];
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

OPCODES_
math1_op_ret(s8 t, s8 flag)
{
	if(t == '+' && flag == 'i') return OPCODE_ADD_REG_IMM;
	else if(t == '+' && flag == 'r') return OPCODE_ADD_REG_REG;
	else if(t == 'x' && flag == 'i') return OPCODE_MUL_REG_IMM;
	else if(t == 'x' && flag == 'r') return OPCODE_MUL_REG_REG;
}

OPCODES_
math2_op_ret(s8 t, s8 flag1, s8 flag2)
{
	if(t == '-')
	{
		if(flag2 == 'i') return OPCODE_SUB_REG_IMM;
		else if(flag1 == 'i') return OPCODE_SUB_IMM_REG;
		return OPCODE_SUB_REG_REG;
	}
	else if(t == '/')
	{
		if(flag2 == 'i') return OPCODE_DIV_REG_IMM;
		else if(flag1 == 'i') return OPCODE_DIV_IMM_REG;
		return OPCODE_DIV_REG_REG;
	}
}
