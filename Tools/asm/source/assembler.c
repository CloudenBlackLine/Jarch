/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



 #include "../include/assembler.h"
#include "../include/util.h"
#include "../../include/jarch.h"

u64 text_base	= ROM_TEXT_BASE;
u64 data_base	= RAM_DATA_BASE;
u64 bss_base 	= RAM_BSS_BASE;
u64 const_base 	= RAM_CONST_BASE;

void
assembler()
{
	Section section = SECTION_NO;
	s8* p = malloc(MAX_ASM_SYNTAX_BUFFER);
	s8* c = p;

	skip_ws(p, acode);
	asm_syntax_size = strlen(p);

	first_pass(p, asm_syntax_size);
	p = c;
	asm_syntax_size = strlen(p);

	text_offset = 0;
	const_offset = 0;
	data_offset = 0;

	second_pass(p, asm_syntax_size);
	return;
}

void
first_pass(s8* p, u64 len)
{
	Section section = SECTION_NO;
	s8 line[MAX_ASM_LINE_LENGTH];
	bool end = true;
	bool t = false;

	while(*p!='\0' && len!=0)
	{
		get_name(line, p);
		if(start_contains(line, "~text")) section = SECTION_TEXT, p+=5;
		else if(start_contains(line, "~data")) section = SECTION_DATA, p+=5;
		else if(start_contains(line, "~const")) section = SECTION_CONST, p+=6;
		else if(start_contains(line, "~bss")) section = SECTION_BSS, p+=4;

		switch(section)
		{
			case SECTION_TEXT:
			{
				p = get_label(line, p, section);
				if(*p == ':')
				{
					t = true;
					strcpy(labels[symbol_count].name, line);
					labels[symbol_count].base_addr = text_base;
					labels[symbol_count].offset = 0;
					labels[symbol_count].section = section;
					symbol_count++;
				}
				if(*p == ';')
				{
					if(t == true) { t = false, text_base+=0x8; }
					else labels[symbol_count-1].offset++, text_base+=0x8;
				}
				p++;
				break;
			}
			case SECTION_CONST:
			{
				if(*p == '.') p++;
				p = get_label(line, p, section);
				if(*p == '=')
				{
					p++;
					strcpy(labels[symbol_count].name, line);
					labels[symbol_count].section = section;
					labels[symbol_count].base_addr = const_base, const_base+=0x8;
					labels[symbol_count++].offset = 0;
				}
				if(*p == '[') p++, end = false;
				if(*p == '#') p++;
				if(*p == ',') p++, labels[symbol_count-1].offset++, const_base+=0x8;
				else p++;
				break;
			}
			case SECTION_DATA:
			{
				p++;
				p = get_label(line, p, section);
				if(*p == '=')
				{
					p++;
					strcpy(labels[symbol_count].name, line);
					labels[symbol_count].section = section;
					labels[symbol_count].base_addr = data_base, data_base+=0x8;
					labels[symbol_count++].offset = 0;
				}
				if(*p == '[') p++, end = false;
				if(*p == '#') p++;
				if(*p == ',') p++, labels[symbol_count-1].offset++, data_base+=0x8;
				else p++;
				break;
			}
			case SECTION_BSS:
			{
				p++;
				p = get_label(line, p, section);
				if(*p == '[')
				{
					p++;
					strcpy(labels[symbol_count].name, line);
					labels[symbol_count].section = SECTION_BSS;
					labels[symbol_count].base_addr = bss_base, bss_base+=0x8;
				}
				if(*p == '#')
				{
					p++; u64 t = get_hex(&p);
					labels[symbol_count++].offset = (t-1);
					bss_base += 0x8*(t-1);
				}
				if(*p == ']') p++;
				if(*p == ';') p++;
				break;
			}
			default: p++; break;
		}

		len--;
	}
	return;
}

void
second_pass(s8* p, u64 len)
{
	Section section = SECTION_NO;
	s8 line[MAX_ASM_LINE_LENGTH];
	while(*p!='\0' && len!=0) 
	{
		get_label(line, p, SECTION_NO);
		if(start_contains	  (line, "~text"))	section = SECTION_TEXT, p+=5;
		else if(start_contains(line, "~data")) 	section = SECTION_DATA, p+=5;
		else if(start_contains(line, "~const")) section = SECTION_CONST, p+=6;
		else if(start_contains(line, "~bss")) 	section = SECTION_BSS, p+=4;

		switch(section)
		{
			case SECTION_TEXT: p = section_text_line(p), p++; break;
			case SECTION_DATA: p = section_data_line(p), p++; break;
			case SECTION_CONST: p = section_const_line(p), p++; break;
			case SECTION_BSS: p = get_line(line, p), p++; break;
		}

		len--;
	}
	return;
}
s8*
section_const_line(s8* p)
{
	while(*p != ';')
	{
		if(*p == '[') p++;
		if(*p == '#')
		{
			p++;
			const_buffer[const_offset++] = get_hex(&p);
			if(*p == ']')
			{
				p++;
				break;
			}
		}
		else p++;
	}
	return p;
}

s8*
section_data_line(s8* p)
{
	while(*p != ';')
	{
		if(*p == '[') p++;
		if(*p == '#')
		{
			p++;
			data_buffer[data_offset++] = get_hex(&p);
			if(*p == ']')
			{
				p++;
				break;
			}
		}
		else p++;
	}
	return p;
}

s8*
section_text_line(s8* p)
{
	if(*p == ':') return p++;
	u64 r1=0, r2=0, r3=0, offset=0;
	u32 imm = 0;
	s8 t = ' ';
	OPCODES_ opcode;
	switch(*p)
	{
		case '#':
			p++;
			offset = get_hex(&p);
			if(*p == ';') text_buffer[text_offset++] = offset;
			break;
		case '$':
			p++;
			r1 = get_reg(&p);
			if(*p == '=') p++;
			switch(*p)
			{
				case '$':
					p++, r2 = get_reg(&p);
					if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_MOV_REG, r1, r2, r3, imm);
					break;
				case '#':
					p++, imm = get_hex(&p);
					if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_MOV_IMM, r1, r2, r3, imm);
					break;
				case '.':
					p++;
					u64 offset = 0;
					s8 name[64];
					p = get_name(name, p);
					if(*p == '[')
					{
						p+=2;
						offset = get_hex(&p);
						if(*p == ']') p++;
					}
					imm = find_label(name);
					if(imm == UN_DEFINED_LABEL)
					{
						strcpy(relocate_buffer[relocate_offset].name, name);
						relocate_buffer[relocate_offset].section = SECTION_TEXT;
						relocate_buffer[relocate_offset++].offset = text_offset;
					}
					if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_MOV_ADDR_VAL, r1, r2, r3, imm+(8*offset));
					break;
				case '*':
					p++;
					if(*p == '$')
					{
						p++, r2 = get_reg(&p);
						if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_MOV_REG_VAL, r1, r2, r3, imm);
					}
					else if(*p == '.')
					{
						p++;
						u64 offset = 0;
						s8 name[64];
						p = get_name(name, p);
						if(*p == '[')
						{
							p+=2;
							offset = get_hex(&p);
							if(*p == ']') p++;
						}
						imm = find_label(name);
						if(imm == UN_DEFINED_LABEL)
						{
							strcpy(relocate_buffer[relocate_offset].name, name);
							relocate_buffer[relocate_offset].section = SECTION_TEXT;
							relocate_buffer[relocate_offset++].offset = text_offset;
						}
						if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_MOV_LABEL_ADDR_VAL, r1, r2, r3, imm+(8*offset));
					}
					else
					{
						imm = get_hex(&p);
						if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_MOV_ADDR_VAL, r1, r2, r3, imm);
					}
					break;
				default: p++; break;
			}
			break;
		case '.':
			p++;
			s8 name[64];
			p = get_name(name, p);
			if(*p == '[')
			{
				p+=2;
				offset = get_hex(&p);
				if(*p == ']') p++;
			}
			if(*p == '=') p++;
			switch(*p)
			{
				case '$':
					p++;
					r1 = get_reg(&p);
					imm = find_label(name);
					if(imm == UN_DEFINED_LABEL)
					{
						strcpy(relocate_buffer[relocate_offset].name, name);
						relocate_buffer[relocate_offset].section = SECTION_TEXT;
						relocate_buffer[relocate_offset++].offset = text_offset;
					}
					if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_ADDR_VAL_REG, r1, r2, r3, imm+(8*offset));
					break;
				case '*':
					p++;
					if(*p == '$')
					{
						p++;
						r1 = get_reg(&p);
						imm = find_label(name);
						if(imm == UN_DEFINED_LABEL)
						{
							strcpy(relocate_buffer[relocate_offset].name, name);
							relocate_buffer[relocate_offset].section = SECTION_TEXT;
							relocate_buffer[relocate_offset++].offset = text_offset;
						}
						if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_ADDR_VAL_REG_VAL, r1, r2, r3, imm+(8*offset));
					}
					break;
			}
			break;
		case '!':
			p++;
			switch(*p)
			{
				case '(':
					p++;
                    if(*p == '$') p++;
                    r1 = get_reg(&p);
                    switch(*p)
                    {
						case '=': p++; opcode = OPCODE_CMP_EQ_ADDR; break;
    	                case '!': p++; opcode = OPCODE_CMP_NE_ADDR; break;
                        case '>': opcode = OPCODE_CMP_LT_ADDR; break;
                        case '<': opcode = OPCODE_CMP_GT_ADDR; break;
                    }
                    p++;
                	if(*p == '$') p++;
                    r2 = get_reg(&p);
                    if(*p == ')') p++;
                    s8 buffer[64];
                	p = get_name(buffer, p);
					imm = find_label(buffer);
					if(imm == UN_DEFINED_LABEL)
					{
						strcpy(relocate_buffer[relocate_offset].name, buffer);
						relocate_buffer[relocate_offset].section = SECTION_TEXT;
						relocate_buffer[relocate_offset++].offset = text_offset;
					}
					if(*p == ';') text_buffer[text_offset++] = packed(opcode, r1, r2, r3, imm);

					break;
				case '*':
					p++;
					imm = get_hex(&p);
					if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_JMP_ADDR, r1, r2, r3, imm);
					break;
				case '$':
					p++;
					r1 = get_reg(&p);
					if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_JMP_REG, r1, r2, r3, imm);
					break;
				default:
					s8 line[64];
					p = get_name(line, p);
					if(!strcmp(line, "halt")) text_buffer[text_offset++] = packed(OPCODE_HALT, r1, r2, r3, imm);
					else if(!strcmp(line, "ret")) text_buffer[text_offset++] = packed(OPCODE_RET, r1, r2, r3, imm);
					else if(!strcmp(line, "push"))
					switch(*p)
					{
						case '$':
							p++;
							r1 = get_reg(&p);
							if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_PUSH_REG, r1, r2, r3, imm);
							break;
						case '.':
							p++;
							p = get_name(line, p);
							if(*p == '[')
							{
								p++; if(*p == '#') p++;
								offset = get_hex(&p);
								if(*p == ']') p++;
							}
							
							imm = find_label(name);
							if(imm == UN_DEFINED_LABEL)
							{
								strcpy(relocate_buffer[relocate_offset].name, name);
								relocate_buffer[relocate_offset].section = SECTION_TEXT;
								relocate_buffer[relocate_offset++].offset = text_offset;
							}
							if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_PUSH_ADDR_VAL, r1, r2, r3, imm+(8*offset));
							break;
						default: p++; break;
					}
					else if(!strcmp(line, "pop"))
					switch(*p)
					{
						case '$':
							p++;
							r1 = get_reg(&p);
							if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_POP_REG, r1, r2, r3, imm);
							break;
						case '.':
							p++;
							p = get_name(line, p);
							if(*p == '[')
							{
								p++; if(*p == '#') p++;
								offset = get_hex(&p);
								if(*p == ']') p++;
							}
							imm = find_label(line);
							if(imm == UN_DEFINED_LABEL)
							{
								strcpy(relocate_buffer[relocate_offset].name, line);
								relocate_buffer[relocate_offset].section = SECTION_TEXT;
								relocate_buffer[relocate_offset++].offset = text_offset;
							}
							if(*p == ';') text_buffer[text_offset++] = packed(OPCODE_POP_ADDR_VAL, r1, r2, r3, imm+(8*offset));
							break;
						default: p++; break;
					}
					else
					{
						imm = find_label(line);
						if(imm == UN_DEFINED_LABEL)
						{
							strcpy(relocate_buffer[relocate_offset].name, line);
							relocate_buffer[relocate_offset].section = SECTION_TEXT;
							relocate_buffer[relocate_offset++].offset = text_offset;
						}
						text_buffer[text_offset++] = packed(OPCODE_JMP_ADDR, r1, r2, r3, imm);
					}
					break;
			}
			break;
		case '+':
		case 'x':
			t = *p++;
			if(*p == '$') p++, r1 = get_reg(&p);
			if(*p == '=') p++;
			if(*p == '$') p++, r2 = get_reg(&p);
			if(*p == ',') p++;
			s8 flag;
			if(*p == '$') p++, r3 = get_reg(&p), flag = 'r';
			else if(*p == '#') p++, imm = get_reg(&p), flag = 'i';
			if(*p == ';') text_buffer[text_offset++] = packed(math1_op_ret(t, flag), r1, r2, r3, imm);
			break;
		case '-':
		case '/':
			t = *p++;
			if(*p == '$') p++, r1 = get_reg(&p);
			if(*p == '=') p++;
			
			s8 flag1;
			if(*p == '$') p++, r2 = get_reg(&p), flag1 = 'r';
			else if(*p == 'i') p++, imm = get_hex(&p), flag1 = 'i';

			if(*p == ',') p++;

			s8 flag2;
			if(*p == '$') p++, r3 = get_reg(&p), flag2 = 'r';
			else if(*p == '#') p++, imm = get_hex(&p), flag2 = 'i';
			if(*p == ';') text_buffer[text_offset++] = packed(math2_op_ret(t, flag1, flag2), r1, r2, r3, imm);
			break;
		case '>':
		case '<':
		case '&':
			p++;
			break;
		default: p++; break;
	}
	r1 = 0, r2 = 0, r3 = 0, imm = 0, offset = 0, t = ' ';
	return p;
}
