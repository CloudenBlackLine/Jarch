#include "./header.h"

#define 	MAX_CODE	8192

void		readfile(s8* filename);
void		writefile(s8* filename);

void		assembler();
s8*		section_init(s8* code);
s8*		section_data(s8* code);
s8*		section_text(s8* code);
s8*		section_unit(s8* code);

void		skip_ws(s8* p);
u64		get_hex(s8** p);
u8		get_reg(s8** p);
s8*		get_name(s8* dest, s8* src);
u32		get_addr(s8* name);
Flag		get_flag(s8* name);
s32		get_index(s8* name);

s32		get_unit_index(s8* name);

u64		packed(u8 r1, u8 r2, u8 r3, u8 r4, u32 imm);
Instruction	unpack(u64 value);


static volatile SystemConfig sys_config=
{
	.entry_point = 0x00000000,
	.max_labels = 300,
	.max_memory = 0x00007d00,

	.text_base_addr = 0x00000000,
	.data_base_addr = 0x00005c00,
	.unit_base_addr = 0x00003000,
	.stack_base_addr = 0x00006000,

	.u8_base_addr = 0x00005c00,
	.u16_base_addr = 0x00005d00,
	.u32_base_addr = 0x00005e00,
	.u64_base_addr = 0x00005f00,
};

Unit		unit_table[0x10];
u64		unit_count = 0;
u64		current_unit = 0;


u64		current_data = 0;

u64		opcodes[0x00007d00/8];
u64		current_addr = 0;

s8		code[MAX_CODE];

Label 		label_table[300];
u64		label_count = 0;

u64		loop_namer = 61;




s32
main(s32 argc, s8* argv[])
{
	if(argc < 2) return 1;
	readfile(argv[1]);
	assembler();
	writefile(argv[1]);
	return 0;
}


void
assembler()
{
	Section section = SECTION_NO;
	s8* p = code;
	skip_ws(p);

	while(*p)
	{
		if(*p == '~')
		{
			p++;
			s8 buffer[32];
			s32 i = 0;
			while(i != 4) buffer[i++] = *p++;
			buffer[i] = 0;
			if(!strcmp(buffer, "init")) section = SECTION_INIT;
			else if(!strcmp(buffer, "data")) section = SECTION_DATA;
			else if(!strcmp(buffer, "text")) section = SECTION_TEXT;
			else if(!strcmp(buffer, "unit")) section = SECTION_UNIT;
			else section = SECTION_NO;
		}
		switch(section)
		{
			case SECTION_INIT: p = section_init(p); break;
			case SECTION_DATA: p = section_data(p); break;
			case SECTION_TEXT: p = section_text(p); break;
			case SECTION_UNIT: p = section_unit(p); break;
			default: p++; section = SECTION_NO; break;
		}
	}

	return;
}


s8*
section_init(s8* p)
{
	u64 current = 1;
	if(*p == '.') p++;
	switch(*p)
	{
		case 'd':
		{
			p+=6;
			if(*p == ':') p++;
			if(*p == '#') p++;
			sys_config.data_base_addr = get_hex(&p);
			sys_config.u8_base_addr = sys_config.data_base_addr + 0;
			sys_config.u16_base_addr = sys_config.data_base_addr + 50;
			sys_config.u32_base_addr = sys_config.data_base_addr + 100;
			sys_config.u64_base_addr = sys_config.data_base_addr + 150;
			if(*p == ';') p++;
			break;
		}
		case 'u':
		{
			p+=6;
			if(*p == ':') p++;
			if(*p == '#') p++;
			sys_config.unit_base_addr = get_hex(&p);
			if(*p == ';') p++;
			break;
		}
		case 't':
		{
			p+=6;
			if(*p == ':') p++;
			if(*p == '#') p++;
			sys_config.text_base_addr = get_hex(&p);
			if(*p == ';') p++;
			break;
		}
		case 's':
		{
			p+=7;
			if(*p == ':') p++;
			if(*p == '#') p++;
			sys_config.stack_base_addr = get_hex(&p);
			if(*p == ';') p++;
			break;
		}
		case 'm':
		{
			p+=6;
			if(*p == ':') p++;
			if(*p == '#') p++;
			sys_config.max_memory = get_hex(&p);
			if(*p == ';') p++;
			break;
		}
		case 'l':
		{
			p+=8;
			if(*p == ':') p++;
			if(*p == '#') p++;
			sys_config.max_labels = get_hex(&p);
			if(*p == ';') p++;
			break;
		}
		default:
		{
			p++;
			break;
		}
	}
	return p;
}

s8*
section_data(s8* p)		// COMPLETE
{
	switch(*p)
	{
		case '.':
		{
			p++;
			s8 buffer[32];
			p = get_name(buffer, p);
			if(*p == '[')
			{
				p+=2;
				strcpy(label_table[label_count].name, buffer);
				label_table[label_count].flag = FLAG_VAR;
				label_table[label_count++].addr = 8*(current_data + (sys_config.data_base_addr / 8));
				current_data = current_data + get_hex(&p);
				opcodes[current_data++ + (sys_config.data_base_addr / 8)] = 0;
				if(*p == ']') p++;
				if(*p == ';') p++;
				break;
			}
			if(*p == '=') p++;
			switch(*p)
			{
				case '[':
				{
					p++;
					strcpy(label_table[label_count].name, buffer);
					label_table[label_count].flag = FLAG_VAR_LIST;
					label_table[label_count].addr = 8*(current_data + (sys_config.data_base_addr / 8));
					while(*p != ']')
					{
						if(*p == '#') p++;
						u64 imm = get_hex(&p);
						if(*p == ',') p++;
						opcodes[current_data++ + (sys_config.data_base_addr / 8)] = imm;
						label_table[label_count].count++;
					}
					if(*p == ']') p++;
					label_count++;
					break;
				}
				case '#':
				{
					p++;
					strcpy(label_table[label_count].name, buffer);
					label_table[label_count].flag = FLAG_VAR;
					label_table[label_count++].addr = 8*(current_data + (sys_config.data_base_addr / 8));
					opcodes[current_data++ + (sys_config.data_base_addr / 8)] = get_hex(&p);
					break;
				}
			}
			if(*p == ';') p++;
			break;
		}
		case '%':
		{
			p++;
			s8 unit_buffer[32];
			s8 obj_buffer[32];
			p = get_name(unit_buffer, p);
			if(*p == ':') p++;
			p = get_name(obj_buffer, p);
			if(*p == ';') p++;
			s32 index = get_unit_index(unit_buffer);
			strcpy(unit_table[index].obj[unit_table[index].obj_count].name, obj_buffer);
			unit_table[index].obj[unit_table[index].obj_count].addr = 8*(current_unit + (sys_config.unit_base_addr / 8));
			unit_table[index].obj[unit_table[index].obj_count++].offset = unit_table[index].var_count + unit_table[index].offset;
			if(unit_table[index].offset != 0) break;
			else{
			for(s32 i=0; i<unit_table[index].var_count + unit_table[index].offset; i++)
			opcodes[current_unit++ + (sys_config.unit_base_addr / 8)] = i;}
			break;
		}
		default: p++; break;
	}
	return p;
}

s8*
section_unit(s8* p)
{
	u32 count = 0;
	u64 t_offset = 0;
	switch(*p)
	{
		case '%':
		{
			p++;
			s8 unit_buffer[32];
			p = get_name(unit_buffer, p);
			strcpy(unit_table[unit_count].unit_name, unit_buffer);
			if(*p == '{') p++;
			while(*p != '}')
			{
				s8 var_buffer[32];
				p = get_name(var_buffer, p);
				if(*p == '[')
				{
					p++;
					if(*p == '#') p++;
					t_offset += get_hex(&p);
					if(*p == ']') p++;
				}
				if(*p == ';') p++;
				strcpy(unit_table[unit_count].var_name[unit_table[unit_count].var_count++], var_buffer);
			}
			unit_table[unit_count].offset = t_offset;
			unit_count++;
			if(*p == '}') p++;
			break;
		}
	}
	return p;
}

s8*
section_text(s8* p)
{
	u8 opcode = 0, r1 = 0, r2 = 0, r3 = 0;
	u32 imm = 0;
	switch(*p)
	{
		case '@':
		{
			p++;
			current_addr = get_hex(&p) / 8;
			if(*p = ':') p++;
			s8 buffer[32];
			p = get_name(buffer, p);
			if(*p == ':') p++;
			switch(get_flag(buffer))
			{
				case FLAG_LABEL:
				{
					strcpy(label_table[label_count].name, buffer);
					label_table[label_count].addr = current_addr;
					label_table[label_count++].flag = FLAG_COMPLETE;
					break;
				}
				case FLAG_ADDR:
				{
					label_table[get_index(buffer)].addr = 8*(current_addr + (sys_config.text_base_addr / 8));
					label_table[get_index(buffer)].flag = FLAG_COMPLETE;
					for(int i=0; i<label_table[get_index(buffer)].count; i++)
					{
						Instruction in = unpack(label_table[get_index(buffer)].opcode[i]);
						opcodes[label_table[get_index(buffer)].location[i]] = packed(in.opcode, in.r1, in.r2, in.r3, (u32)label_table[get_index(buffer)].addr);
					}
					break;
				}
			}
			break;
		}




























		case ':':
		{
			p++;
			s8 buffer[32];
			p = get_name(buffer, p);
			if(*p == ':') p++;
			switch(get_flag(buffer))
			{
				case FLAG_LABEL:
				{
					strcpy(label_table[label_count].name, buffer);
					label_table[label_count].addr = 8*(current_addr + (sys_config.text_base_addr / 8));
					label_table[label_count++].flag = FLAG_COMPLETE;
					break;
				}
				case FLAG_ADDR:
				{
					label_table[get_index(buffer)].addr = 8*(current_addr + (sys_config.text_base_addr / 8));
					label_table[get_index(buffer)].flag = FLAG_COMPLETE;
					for(int i=0; i<label_table[get_index(buffer)].count; i++)
					{
						Instruction in = unpack(label_table[get_index(buffer)].opcode[i]);
						opcodes[label_table[get_index(buffer)].location[i]] = packed(in.opcode, in.r1, in.r2, in.r3, (u32)label_table[get_index(buffer)].addr);
					}
					break;
				}
			}
			break;
		}

















		case '.':
		{
			p++;
			s8 buffer[32];
			u32 offset = 0;
			p = get_name(buffer, p);
			if(*p == '[')
			{
				p++;
				if(*p == '#') p++;
				offset = (8 * get_hex(&p));
				if(*p == ']') p++;
			}
			if(*p == '=') p++;
			switch(*p)
			{
				case '*':
				{
					p++;
					if(*p == '$') p++;
					r1 = get_reg(&p);
					opcode = OPCODE_ADDR_VAL_REG_VAL;
					imm = get_addr(buffer) + offset;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					if(*p == ';') p++;
					break;
				}
				case '$':
				{
					p++;
					if(*p == '$') p++;
					r1 = get_reg(&p);
					opcode = OPCODE_ADDR_VAL_REG;
					imm = get_addr(buffer) + offset;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					if(*p == ';') p++;
					break;
				}
			}
			break;
		}

















		case '$':
		{
			p++;
			r1 = get_reg(&p);
			if(*p == '=') p++;
			switch(*p)
			{
				case '#':
				{
					p++;
					imm = get_hex(&p);
					opcode = OPCODE_MOV_IMM;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					if(*p == ';') p++;
					break;
				}
				case '$':
				{
					p++;
					r2 = get_reg(&p);
					opcode = OPCODE_MOV_REG;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					if(*p == ';') p++;
					break;
				}
				case '.':
				{
					p++;
					s8 buffer[32];
					u32 offset = 0;
					p = get_name(buffer, p);
					if(*p == '[')
					{
						p++;
						if(*p == '#') p++;
						offset = (8 * get_hex(&p));
						if(*p == ']') p++;
					}
					switch(get_flag(buffer))
					{
						case FLAG_VAR:
						{
							imm = get_addr(buffer) + offset;
							opcode = OPCODE_MOV_ADDR_VAL;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
						case FLAG_VAR_LIST:
						{
							imm = get_addr(buffer) + offset;
							opcode = OPCODE_MOV_ADDR_VAL;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
					}
					if(*p == ';') p++;
					break;
				}
				case '@':
				{
					p++;
					switch(*p)
					{
						case '.':
						{
							p++;
							s8 buffer[32];
							p = get_name(buffer, p);
							imm = get_addr(buffer);
							opcode = OPCODE_MOV_IMM;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							if(*p == ';') p++;
							break;
						}
						case ':':
						{
							p++;
							s8 buffer[32];
							p = get_name(buffer, p);
							opcode = OPCODE_MOV_IMM;
							switch(get_flag(buffer))
							{
								case FLAG_LABEL:
								{
									strcpy(label_table[label_count].name, buffer);
									label_table[label_count].flag = FLAG_ADDR;
									label_table[label_count].opcode[label_table[label_count].count] = packed(opcode, r1, r2, r3, imm);
									label_table[label_count].location[label_table[label_count++].count++] = current_addr++ + (sys_config.text_base_addr / 8);
									break;
								}
								case FLAG_ADDR:
								{
									label_table[get_index(buffer)].opcode[label_table[get_index(buffer)].count] = packed(opcode, r1, r2, r3, imm);
									label_table[get_index(buffer)].location[label_table[get_index(buffer)].count++] = current_addr++ + (sys_config.text_base_addr / 8);
									break;
								}
								case FLAG_COMPLETE:
								opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, get_addr(buffer)); break;
							}
							if(*p == ';') p++;
							break;
						}
						case '%':
						{
							p++;
							s8 obj_buffer[32];
							s8 var_buffer[32];
							s32 u_i = 0, temp = 0, o_i = 0, v_i = 0;
							p = get_name(obj_buffer, p);
							if(*p == '.') p++;
							p = get_name(var_buffer, p);
							opcode = OPCODE_MOV_IMM;
							if(*p == ';');
							for(u_i=0; u_i<unit_count; u_i++)
							{
								for(o_i=0; o_i<unit_table[u_i].obj_count; o_i++)
									{
									if(!strcmp(unit_table[u_i].obj[o_i].name, obj_buffer))
									{
										temp = u_i;
										u_i = unit_count;
										break;
									}
								}
							}
							u_i = temp;
							for(v_i=0; v_i<unit_table[u_i].var_count; v_i++)
							if(!strcmp(unit_table[u_i].var_name[v_i], var_buffer))
							break;
							imm = (unit_table[u_i].obj[o_i].addr + (v_i * 8));
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
					}
					break;
				}
				case '%':
				{
					p++;
					s8 obj_buffer[32];
					s8 var_buffer[32];
					s32 u_i = 0, temp = 0, o_i = 0, v_i = 0, offset = 0;
					p = get_name(obj_buffer, p);
					if(*p == '.') p++;
					p = get_name(var_buffer, p);
					opcode = OPCODE_MOV_ADDR_VAL;
					if(*p == '[')
					{
						p++;
						if(*p == '#') p++;
						offset = 8 * get_hex(&p);
						if(*p == ']') p++;
					}
					if(*p == ';') p++;
					for(u_i=0; u_i<unit_count; u_i++)
					{
						for(o_i=0; o_i<unit_table[u_i].obj_count; o_i++)
							{
							if(!strcmp(unit_table[u_i].obj[o_i].name, obj_buffer))
							{
								temp = u_i;
								u_i = unit_count;
								break;
							}
						}
					}
					u_i = temp;
					for(v_i=0; v_i<unit_table[u_i].var_count; v_i++)
					if(!strcmp(unit_table[u_i].var_name[v_i], var_buffer))
					break;
					imm = (unit_table[u_i].obj[o_i].addr + (v_i * 8)) + offset;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
				case '*':
				{
					p++;
					switch(*p)
					{
						case '$':
						{
							p++;
							r2 = get_reg(&p);
							opcode = OPCODE_MOV_REG_VAL;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							if(*p == ';') p++;
							break;
						}
						case ':':
						{
							p++;
							s8 buffer[32];
							p = get_name(buffer, p);
							opcode = OPCODE_MOV_LABEL_ADDR_VAL;
							switch(get_flag(buffer))
							{
								case FLAG_LABEL:
								{
									strcpy(label_table[label_count].name, buffer);
									label_table[label_count].flag = FLAG_ADDR;
									label_table[label_count].opcode[label_table[label_count].count] = packed(opcode, r1, r2, r3, imm);
									label_table[label_count].location[label_table[label_count++].count++] = current_addr++ + (sys_config.text_base_addr / 8);
									break;
								}
								case FLAG_ADDR:
								{
									label_table[get_index(buffer)].opcode[label_table[get_index(buffer)].count] = packed(opcode, r1, r2, r3, imm);
									label_table[get_index(buffer)].location[label_table[get_index(buffer)].count++] = current_addr++ + (sys_config.text_base_addr / 8);
									break;
								}
								case FLAG_COMPLETE:
								opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, get_addr(buffer)); break;
							}
							if(*p == ';') p++;
							break;
						}
						case '.':
						{
							p++;
							u64 offset = 0;
							s8 buffer[32];
							p = get_name(buffer, p);
							if(*p == '[')
							{
								p++;
								if(*p == '#') p++;
								offset = get_hex(&p);
								if(*p == ']') p++;
							}
							imm = get_addr(buffer) + (8 * offset);
							opcode = OPCODE_MOV_LABEL_ADDR_VAL;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							if(*p == ';') p++;
							break;
						}
						case '%':
						{
							p++;
							s8 obj_buffer[32];
							s8 var_buffer[32];
							s32 u_i = 0, temp = 0, o_i = 0, v_i = 0;
							p = get_name(obj_buffer, p);
							if(*p == '.') p++;
							p = get_name(var_buffer, p);
							opcode = OPCODE_MOV_LABEL_ADDR_VAL;
							if(*p == ';');
							for(u_i=0; u_i<unit_count; u_i++)
							{
								for(o_i=0; o_i<unit_table[u_i].obj_count; o_i++)
									{
									if(!strcmp(unit_table[u_i].obj[o_i].name, obj_buffer))
									{
										temp = u_i;
										u_i = unit_count;
										break;
									}
								}
							}
							u_i = temp;
							for(v_i=0; v_i<unit_table[u_i].var_count; v_i++)
							if(!strcmp(unit_table[u_i].var_name[v_i], var_buffer))
							break;
							imm = (unit_table[u_i].obj[o_i].addr + (v_i * 8));
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
						default:
						{
							imm = get_hex(&p);
							opcode = OPCODE_MOV_ADDR_VAL;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							if(*p == ';') p++;
							break;
						}
					}
					if(*p == ';') p++;
					break;
				}
				case ':':
				{
					p++;
					s8 buffer[32];
					p = get_name(buffer, p);
					opcode = OPCODE_MOV_ADDR_VAL;
					switch(get_flag(buffer))
					{
						case FLAG_LABEL:
						{
							strcpy(label_table[label_count].name, buffer);
							label_table[label_count].flag = FLAG_ADDR;
							label_table[label_count].opcode[label_table[label_count].count] = packed(opcode, r1, r2, r3, imm);
							label_table[label_count].location[label_table[label_count++].count++] = current_addr++ + (sys_config.text_base_addr / 8);
							break;
						}
						case FLAG_ADDR:
						{
							label_table[get_index(buffer)].opcode[label_table[get_index(buffer)].count] = packed(opcode, r1, r2, r3, imm);
							label_table[get_index(buffer)].location[label_table[get_index(buffer)].count++] = current_addr++ + (sys_config.text_base_addr / 8);
							break;
						}
						case FLAG_COMPLETE:
						opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, get_addr(buffer)); break;
					}
					if(*p == ';') p++;
					break;
				}
				default: p++; break;
			}
			break;
		}
























		case '*':
		{
			p++;
			switch(*p)
			{
				case '$':
				{
					p++;
					r1 = get_reg(&p);
					if(*p == '=') p++;
					switch(*p)
					{
						case '$':
						{
							p++;
							r2 = get_reg(&p);
							if(*p == ';') p++;
							opcode = OPCODE_REG_VAL_REG;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
						case '#':
						{
							p++;
							imm = get_hex(&p);
							if(*p == ';') p++;
							opcode = OPCODE_REG_VAL_IMM;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
						case '.':
						{
							p++;
							s8 buffer[32];
							p = get_name(buffer, p);
							if(*p == ';') p++;
							imm = get_addr(buffer);
							opcode = OPCODE_REG_VAL_ADDR_VAL;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
						case '%':
						{
							p++;
							break;
						}
						case '*':
						{
							p++;
							switch(*p)
							{
								case '$':
								{
									p++;
									r2 = get_reg(&p);
									if(*p == ';') p++;
									opcode = OPCODE_REG_VAL_REG_VAL;
									opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
									break;
								}
								case '%':
								{
									p++;
									break;
								}
								default:
								{
									imm = get_hex(&p);
									if(*p == ';') p++;
									opcode = OPCODE_REG_VAL_ADDR_VAL;
									opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
									break;
								}
							}
							break;
						}
					}
					break;
				}
				case '.':
				{
					p++;
					s8 buffer[32];
					p = get_name(buffer, p);
					if(*p == '=') p++;
					switch(*p)
					{
						case '$':
						{
							p++;
							r1 = get_reg(&p);
							opcode = OPCODE_LABEL_ADDR_VAL_REG;
							imm = get_addr(buffer);
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
						case '*':
						{
							p++;
							if(*p == '$') p++;
							r1 = get_reg(&p);
							opcode = OPCODE_LABEL_ADDR_VAL_REG_VAL;
							imm = get_addr(buffer);
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
					}
					if(*p == ';') p++;
					break;
				}
				case '%':
				{
					p++;
					s8 obj_buffer[32];
					s8 var_buffer[32];
					s32 u_i = 0, temp = 0, o_i = 0, v_i = 0, offset = 0;
					p = get_name(obj_buffer, p);
					if(*p == '.') p++;
					p = get_name(var_buffer, p);
					if(*p == '[')
					{
						p++;
						if(*p == '#') p++;
						offset = 8 * get_hex(&p);
						if(*p == ']') p++;
					}
					if(*p == '=') p++;
					switch(*p)
					{
						case '$':
						{
							p++;
							r1 = get_reg(&p);
							opcode = OPCODE_LABEL_ADDR_VAL_REG;
							break;
						}
						case '*':
						{
							p++;
							if(*p == '$') p++;
							r1 = get_reg(&p);
							opcode = OPCODE_LABEL_ADDR_VAL_REG_VAL;
							break;
						}
					}
					for(u_i=0; u_i<unit_count; u_i++)
					{
						for(o_i=0; o_i<unit_table[u_i].obj_count; o_i++)
							{
							if(!strcmp(unit_table[u_i].obj[o_i].name, obj_buffer))
							{
								temp = u_i;
								u_i = unit_count;
								break;
							}
						}
					}
					u_i = temp;
					for(v_i=0; v_i<unit_table[u_i].var_count; v_i++)
					if(!strcmp(unit_table[u_i].var_name[v_i], var_buffer))
					break;
					imm = (unit_table[u_i].obj[o_i].addr + (v_i * 8)) + offset;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					if(*p == ';') p++;
					break;
				}
				default:
				{
					imm = get_hex(&p);
					if(*p == '=') p++;
					switch(*p)
					{
						case '$':
						{
							p++;
							r1 = get_reg(&p);
							if(*p == ';') p++;
							opcode = OPCODE_ADDR_VAL_REG;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
						case '*':
						{
							p++;
							if(*p == '$') p++;
							r1 = get_reg(&p);
							if(*p == ';') p++;
							opcode = OPCODE_ADDR_VAL_REG_VAL;
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
							break;
						}
					}
					break;
				}
			}
			break;
		}














		case '#':
		{
			p++;
			opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = (u64)get_hex(&p);
			if(*p == ';') p++;
			break;
		}

















		case '%':
		{
			p++;
			s8 obj_buffer[32];
			s8 var_buffer[32];

			u32 offset = 0;
			s32 u_i = 0;
			s32 temp = 0;
			s32 o_i = 0;
			s32 v_i = 0;

			p = get_name(obj_buffer, p);

			for(u_i=0; u_i<unit_count; u_i++)
			{
				for(o_i=0; o_i<unit_table[u_i].obj_count; o_i++)
				{
					if(!strcmp(unit_table[u_i].obj[o_i].name, obj_buffer))
					{
						temp = u_i;
						u_i = unit_count;
						break;
					}
				}
			}
			if(*p == '.') p++;
			p = get_name(var_buffer, p);
			if(*p == '[')
			{
				p++;
				if(*p == '#') p++;
				offset = 8 * get_hex(&p);
				if(*p == ']') p++;
			}
			if(*p == '=') p++;

			u_i = temp;
			for(v_i=0; v_i<unit_table[u_i].var_count; v_i++)
			if(!strcmp(unit_table[u_i].var_name[v_i], var_buffer))
			break;

			switch(*p)
			{
				case '$':
				{
					p++;
					r1 = get_reg(&p);
					imm = (unit_table[u_i].obj[o_i].addr + (v_i * 8)) + offset;
					opcode = OPCODE_ADDR_VAL_REG;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
			}

			if(*p == ';') p++;
			break;
		}























		case '!':
		{
			p++;
			switch(*p)
			{
				case '(':
				{
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
					if(*p == ')') p++; if(*p == ':') p++;
					s8 buffer[32];
					p = get_name(buffer, p);
					switch(get_flag(buffer))
					{
						case FLAG_LABEL:
						{
							strcpy(label_table[label_count].name, buffer);
							label_table[label_count].flag = FLAG_ADDR;
							label_table[label_count].opcode[label_table[label_count].count] = packed(opcode, r1, r2, r3, imm);
							label_table[label_count].location[label_table[label_count++].count++] = (current_addr++ + (sys_config.text_base_addr / 8));
							break;
						}
						case FLAG_ADDR:
						{
							label_table[get_index(buffer)].opcode[label_table[get_index(buffer)].count] = packed(opcode, r1, r2, r3, imm);
							label_table[get_index(buffer)].location[label_table[get_index(buffer)].count++] = (current_addr++ + (sys_config.text_base_addr / 8));
							break;
						}
						case FLAG_COMPLETE:
						{
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, get_addr(buffer));
							break;
						}
					}
					if(*p == ';') p++;
					break;
				}
				case 'r':
				{
					p+=3;
					opcode = OPCODE_RET;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					if(*p == ';') p++;
					break;
				}
				case 'h':
				{
					p+=4;
					opcode = OPCODE_HALT;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					if(*p == ';') p++;
					break;
				}
				case 'p':
				{
					p++;
					switch(*p)
					{
						case 'o':
						{
							p+=2;
							switch(*p)
							{
								case '$':
								{
									p++;
									r1 = get_reg(&p);
									opcode = OPCODE_POP_REG;
									opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
									break;
								}
							}
							break;
						}
						case 'u':
						{
							p+=3;
							switch(*p)
							{
								case '#':
								{
									p++;
									imm = get_hex(&p);
									opcode = OPCODE_PUSH_IMM;
									opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
									break;
								}
								case '$':
								{
									p++;
									r1 = get_reg(&p);
									opcode = OPCODE_PUSH_REG;
									opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
									break;
								}
							}
							break;
						}
					}
					if(*p == ';') p++;
					break;
				}
				case '.':
				case ':':
				{
					p++;
					s8 buffer[32];
					p = get_name(buffer, p);
					switch(get_flag(buffer))
					{
						case FLAG_LABEL:
						{
							strcpy(label_table[label_count].name, buffer);
							label_table[label_count].flag = FLAG_ADDR;
							label_table[label_count].opcode[label_table[label_count].count] = packed(OPCODE_JMP_ADDR, r1, r2, r3, imm);
							label_table[label_count].location[label_table[label_count++].count++] = (current_addr++ + (sys_config.text_base_addr / 8));
							break;
						}
						case FLAG_ADDR:
						{
							label_table[get_index(buffer)].opcode[label_table[get_index(buffer)].count] = packed(OPCODE_JMP_ADDR, r1, r2, r3, imm);
							label_table[get_index(buffer)].location[label_table[get_index(buffer)].count++] = (current_addr++ + (sys_config.text_base_addr / 8));
							break;																								/////////////////////////
						}
						case FLAG_VAR:
						case FLAG_COMPLETE:
						{
							opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(OPCODE_JMP_ADDR, r1, r2, r3, get_addr(buffer));
							break;
						}
					}
					if(*p == ':' || *p == ';') p++;
					break;
				}
				case '$':
				{
					p++;
					r1 = get_reg(&p);
					opcode = OPCODE_JMP_REG;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					if(*p == ';') p++;
					break;
				}
				default:
				{
					imm = get_hex(&p);
					opcode = OPCODE_JMP_ADDR;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					if(*p == ';') p++;
					break;
				}
			}
			break;
		}

























		case '>':
		{
			p+=2;
			if(*p == '$') p++;
			r1 = get_reg(&p);
			if(*p == '=') p++;
			switch(*p)
			{
				case '$':
				{
					p++;
					r2 = get_reg(&p);
					if(*p == ',') p++;
					switch(*p)
					{
						case '$': p++; r3 = get_reg(&p); opcode = OPCODE_RSHIFT_REG_REG; break;
						case '#': p++; imm = get_hex(&p); opcode = OPCODE_RSHIFT_REG_IMM; break;
					}
					if(*p == ';') p++;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
				case '#':
				{
					p++;
					imm = get_hex(&p);
					if(*p == ',') p++;
					if(*p == '$') p++;
					r3 = get_reg(&p);
					opcode = OPCODE_RSHIFT_IMM_REG;
					if(*p == ';') p++;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
			}
			break;
		}



















		case '<':
		{
			p+=2;
			if(*p == '$') p++;
			r1 = get_reg(&p);
			if(*p == '=') p++;
			switch(*p)
			{
				case '$':
				{
					p++;
					r2 = get_reg(&p);
					if(*p == ',') p++;
					switch(*p)
					{
						case '$': p++; r3 = get_reg(&p); opcode = OPCODE_LSHIFT_REG_REG; break;
						case '#': p++; imm = get_hex(&p); opcode = OPCODE_LSHIFT_REG_IMM; break;
					}
					if(*p == ';') p++;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
				case '#':
				{
					p++;
					imm = get_hex(&p);
					if(*p == ',') p++;
					if(*p == '$') p++;
					r3 = get_reg(&p);
					opcode = OPCODE_LSHIFT_IMM_REG;
					if(*p == ';') p++;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
			}
			break;
		}



















		case '|':
		{
			p++;
			if(*p == '$') p++;
			r1 = get_reg(&p);
			if(*p == '=') p++;
			switch(*p)
			{
				case '$':
				{
					p++;
					r2 = get_reg(&p);
					if(*p == ',') p++;
					switch(*p)
					{
						case '$': p++; r3 = get_reg(&p); opcode = OPCODE_OR_REG_REG; break;
						case '#': p++; imm = get_hex(&p); opcode = OPCODE_OR_REG_IMM; break;
					}
					if(*p == ';') p++;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
				case '#':
				{
					p++;
					imm = get_hex(&p);
					if(*p == ',') p++;
					if(*p == '$') p++;
					r3 = get_reg(&p);
					opcode = OPCODE_OR_IMM_REG;
					if(*p == ';') p++;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
			}
			break;
		}


















		case '+':
		{
			p++;
			if(*p == '$') p++;
			r1 = get_reg(&p);
			if(*p == '=') p++;
			if(*p == '$') p++;
			r2 = get_reg(&p);
			if(*p == ',') p++;
			switch(*p)
			{
				case '$': p++; r3 = get_reg(&p); opcode = OPCODE_ADD_REG_REG; break;
				case '#': p++; imm = get_hex(&p); opcode = OPCODE_ADD_REG_IMM; break;
			}
			if(*p == ';') p++;
			opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
			break;
		}






















		case 'x':
		{
			p++;
			if(*p == '$') p++;
			r1 = get_reg(&p);
			if(*p == '=') p++;
			if(*p == '$') p++;
			r2 = get_reg(&p);
			if(*p == ',') p++;
			switch(*p)
			{
				case '$': p++; r3 = get_reg(&p); opcode = OPCODE_MUL_REG_REG; break;
				case '#': p++; imm = get_hex(&p); opcode = OPCODE_MUL_REG_IMM; break;
			}
			if(*p == ';') p++;
			opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
			break;
		}


















		case '-':
		{
			p++;
			if(*p == '$') p++;
			r1 = get_reg(&p);
			if(*p == '=') p++;
			switch(*p)
			{
				case '$':
				{
					p++;
					r2 = get_reg(&p);
					if(*p == ',') p++;
					switch(*p)
					{
						case '$': p++; r3 = get_reg(&p); opcode = OPCODE_SUB_REG_REG; break;
						case '#': p++; imm = get_hex(&p); opcode = OPCODE_SUB_REG_IMM; break;
					}
					if(*p == ';') p++;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
				case '#':
				{
					p++;
					imm = get_hex(&p);
					if(*p == ',') p++;
					if(*p == '$') p++;
					r3 = get_reg(&p);
					opcode = OPCODE_SUB_IMM_REG;
					if(*p == ';') p++;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
			}
			break;
		}















		case '/':
		{
			p++;
			if(*p == '$') p++;
			r1 = get_reg(&p);
			if(*p == '=') p++;
			switch(*p)
			{
				case '$':
				{
					p++;
					r2 = get_reg(&p);
					if(*p == ',') p++;
					switch(*p)
					{
						case '$': p++; r3 = get_reg(&p); opcode = OPCODE_DIV_REG_REG; break;
						case '#': p++; imm = get_hex(&p); opcode = OPCODE_DIV_REG_IMM; break;
					}
					if(*p == ';') p++;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
				case '#':
				{
					p++;
					imm = get_hex(&p);
					if(*p == ',') p++;
					if(*p == '$') p++;
					r3 = get_reg(&p);
					opcode = OPCODE_DIV_IMM_REG;
					if(*p == ';') p++;
					opcodes[current_addr++ + (sys_config.text_base_addr / 8)] = packed(opcode, r1, r2, r3, imm);
					break;
				}
			}
			break;
		}
		case '{': p++; break;
		case '}': p++; break;
		default: p++; break;
	}
	return p;
}

void
readfile(s8* filename)
{
	s32 fd = open(filename, O_RDONLY);
	if(fd < 0) return;
	s32 size = read(fd, code, MAX_CODE);
	if(size < 0) { close(fd); return; }
	close(fd);
	code[size] = 0;
}


void
writefile(s8* filename)
{
	s32 i = 0, current = 1;
	s8 buffer[32];
	while(*filename!='\0' && *filename != '.')
	buffer[i++] = *filename++;
	buffer[i++] = '.';
	buffer[i++] = 'j';
	buffer[i++] = 'b';
	buffer[i++] = 'i';
	buffer[i++] = 'n';
	buffer[i] = '\0';
	FILE* f = fopen(buffer, "w");
	if(!f) return;
	for(u32 i=0; i<sys_config.max_memory/8; i++)
	{
		Instruction a = unpack(opcodes[i]);
		fprintf(f, "%02x%02x%02x%02x%08x;\n", a.opcode, a.r1, a.r2, a.r3, a.imm);
	}
	fclose(f);
}

void
skip_ws(s8* code)
{
	u32 i = 0, j = 0;
	while(code[i])
	{
		if(code[i] != ' ' && code[i] != '\t' &&  code[i] != '\n' && code[i] != '\r') code[j++] = code[i];
		i++;
	}
	code[j] = 0;
}


u64
get_hex(s8** p)
{
	u64 value = 0;
	while
	(
		(**p >= '0' && **p <= '9') ||
		(**p >= 'A' && **p <= 'F') ||
		(**p >= 'a' && **p <= 'f')
	)
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
	if(*str >= '0' && *str <= '9')
	{
		value = (u8)(*str - '0');
		*p = str + 1;
		return value;
	}

	if(str[0] == 's' && str[1] == 'p')
	{
		*p = str + 2;
		return 0x0a;
	}
	if(str[0] == 'p' && str[1] == 'c')
	{
		*p = str + 2;
		return 0x0b;
	}
	if(str[0] == 'l' && str[1] == 'r')
	{
		*p = str + 2;
		return 0x0c;
	}
	return value;
}

s8*
get_name(s8* dest, s8* src)
{
	while(*src && *src != ':' && *src != ';' && *src != '=' && *src != ',' && *src != '[' && *src != '{' && *src != '.' && *src != '(')
	*dest++ = *src++;
	*dest = 0;
	return src;
}

u32
get_addr(s8* n) { for(s32 i=0; i<label_count; i++) if(!strcmp(label_table[i].name, n)) return label_table[i].addr; return 1; }

Flag
get_flag(s8* n) { for(s32 i=0; i<label_count; i++) if(!strcmp(label_table[i].name, n)) return label_table[i].flag; return FLAG_LABEL; }

s32
get_index(s8* n) { for(s32 i=0; i<label_count; i++) if(!strcmp(label_table[i].name, n)) return i; return -1; }

s32
get_unit_index(s8* n) { for(s32 i=0; i<unit_count; i++) if(!strcmp(unit_table[i].unit_name, n)) return i; return -1; }

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
