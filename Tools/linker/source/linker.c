#include "../include/linker.h"
#include "../include/util.h"

static int	_f_entry = 1;

void
linker(s32 argc, s8* argv[])
{
	s8* out_file;
	file_count++;

	file = malloc(sizeof(File) * argc);

	// Get each files content
	{
	for(u32 i=1; i<argc; i++)
	{
		if(!strcmp(argv[i], "-o"))
		{
			out_file = malloc(strlen(argv[i]));
			strcpy(out_file, argv[++i]);
		} else {

		// read file into loc
		s8* loc = readfile(argv[i]);

		strcpy(file[file_count].name, argv[i]);
		file[file_count].index = i;

		// Create Symbol Table, and find file with _start (entry point)
		loc = read_labels(loc, i);


		// the rest of the object file
		loc = read_raw_hex(loc);
		read_relocation(loc);


		file_count++;
	}}
	}

	// Get hexb_buffer size
	u64	hexb_size = 0;
	for(u32 i=1; i<file_count; i++)
	{
		hexb_size += file[i].t_count;
		hexb_size += file[i].d_count;
		hexb_size += file[i].c_count;
	}
	hex_buffer = malloc((sizeof(Opcode) * hexb_size)+1);

	write_text();
	write_data();
	write_const();
	do_labels();
	do_relocate();

	// DONE

	writefile(out_file);
}

s8*
read_labels(s8* p, u32 i)
{
	if(start_contains(p, "(label)")) p+=8;
	while(*p!='(')
	{
		s8* buffer = malloc(256);
		p = read_line(p, buffer);
		//printf("%s\n", buffer);
		s8* name = malloc(64);
		buffer = read_splice(buffer, name);

		if(!strcmp(name, "_start")) _f_entry = i;

		strcpy(symbol_buffer[symbol_count].name, name);
		symbol_buffer[symbol_count].section = (Section)(*buffer++ - '0'), buffer++;
		buffer = read_splice(buffer, name);
		symbol_buffer[symbol_count].base_addr = read_hex(&name);
		buffer = read_splice(buffer, name);
		symbol_buffer[symbol_count++].offset = read_hex(&name);
	}
	return p;
}

s8*
read_raw_hex(s8* p)
{
	if(start_contains(p, "(text)")) p+=7;
	while(*p!='(')
	{
		s8* buffer = malloc(64);
		p = read_line(p, buffer);
		file[file_count].text_[file[file_count].t_count++] = read_hex(&buffer);
		//printf("%016lx\n", file[file_count].text_[file[file_count].t_count++]);
	}
	if(start_contains(p, "(data)")) p+=7;
	while(*p!='(')
	{
		s8* buffer = malloc(64);
		p = read_line(p, buffer);
		file[file_count].data_[file[file_count].d_count++] = read_hex(&buffer);
		//printf("%016lx\n", file[file_count].data_[file[file_count].d_count++]);
	}
	if(start_contains(p, "(const)")) p+=8;
	while(*p!='(')
	{
		s8* buffer = malloc(64);
		p = read_line(p, buffer);
		file[file_count].const_[file[file_count].c_count++] = read_hex(&buffer);
		//printf("%016lx\n", file[file_count].const_[file[file_count].c_count++]);
	}
	return p;
}

void
read_relocation(s8* p)
{
	if(start_contains(p, "(relocate)")) p+=11;
	while(*p!='\0')
	{
		s8* buffer = malloc(256);
		p = read_line(p, buffer);
		//printf("%s\n", buffer);
		s8* name = malloc(64);
		buffer = read_splice(buffer, name);

		strcpy(relocate[relocate_count].name, name);
		buffer = read_splice(buffer, name);
		relocate[relocate_count].section = (Section)(*buffer++ - '0'), buffer++;
		buffer = read_splice(buffer, name);
		relocate[relocate_count++].offset = read_hex(&name);
	}
	return;
}

void
write_text()
{
	u64	t_base = ROM_TEXT_BASE;

	for(u64 i=0; i<file[_f_entry].t_count; i++)
	{
		hex_buffer[hexb_count].addr = t_base, t_base+=0x8;
		hex_buffer[hexb_count++].value = file[_f_entry].text_[i];
	}

	for(u64 f=0; f<file_count; f++)
	if(f!=_f_entry)
	for(u64 i=0; i<file[f].t_count; i++)
	{
		hex_buffer[hexb_count].addr = t_base, t_base+=0x8;
		hex_buffer[hexb_count++].value = file[f].text_[i];
	}
}

void
write_data()
{
	u64	d_base = RAM_DATA_BASE;

	for(u64 i=0; i<file[_f_entry].d_count; i++)
	{
		hex_buffer[hexb_count].addr = d_base, d_base+=0x8;
		hex_buffer[hexb_count++].value = file[_f_entry].data_[i];
	}

	for(u64 f=0; f<file_count; f++)
	if(f!=_f_entry)
	for(u64 i=0; i<file[f].d_count; i++)
	{
		hex_buffer[hexb_count].addr = d_base, d_base+=0x8;
		hex_buffer[hexb_count++].value = file[f].data_[i];
	}
}

void
write_const()
{
	u64	c_base = RAM_CONST_BASE;

	for(u64 i=0; i<file[_f_entry].c_count; i++)
	{
		hex_buffer[hexb_count].addr = c_base, c_base+=0x8;
		hex_buffer[hexb_count++].value = file[_f_entry].const_[i];
	}

	for(u64 f=0; f<file_count; f++)
	if(f!=_f_entry)
	for(u64 i=0; i<file[f].c_count; i++)
	{
		hex_buffer[hexb_count].addr = c_base, c_base+=0x8;
		hex_buffer[hexb_count++].value = file[f].const_[i];
	}
	// for(u64 i=0; i<hexb_count; i++)
	// printf("%016lx:%016lx;\n", hex_buffer[i].addr, hex_buffer[i].value);
}

void
do_labels()
{
	return;
}

void
do_relocate()
{

	return;
}
