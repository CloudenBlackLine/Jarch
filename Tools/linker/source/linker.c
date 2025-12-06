#include "../include/linker.h"
#include "../include/util.h"

void
linker(s32 argc, s8* argv[])
{
	for(u32 i=1; i<argc; i++)
	{
		s8* loc = '\0';
		// Setup File object
		strcpy(file[file_count].name, argv[i]);
		file[file_count].index = file_count;

		//Generate lists, for each file, and symbol table.
		loc = read_labels(loc);
		loc = read_raw_hex(loc);
		read_relocation(loc);
		file_count++; 
		free(loc);
	}



/*  Writing the output file 
	s8 out_file[64];
	u32 i = 0;
	while(i<argc)
	if(!strcmp(argv[i], "-o"))
	{
		i++;
		strcpy(out_file, argv[i]);
		break;
	}
	else i++;
	//writefile(out_file);*/

}

s8*
read_labels(s8* p)
{
	if(start_contains(p, "(label)")) p+=8;
	while(*p!='(')
	{
		s8* buffer = malloc(256);
		p = read_line(p, buffer);
		printf("%s\n", buffer);
		s8* name = malloc(64);
		buffer = read_splice(buffer, name);

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
		file[file_count].text_[file[file_count].t_count] = read_hex(&buffer);
		printf("%016lx\n", file[file_count].text_[file[file_count].t_count++]);
	}
	if(start_contains(p, "(data)")) p+=7;
	while(*p!='(')
	{
		s8* buffer = malloc(64);
		p = read_line(p, buffer);
		file[file_count].data_[file[file_count].d_count] = read_hex(&buffer);
		printf("%016lx\n", file[file_count].data_[file[file_count].d_count++]);
	}
	if(start_contains(p, "(const)")) p+=8;
	while(*p!='(')
	{
		s8* buffer = malloc(64);
		p = read_line(p, buffer);
		file[file_count].const_[file[file_count].c_count] = read_hex(&buffer);
		printf("%016lx\n", file[file_count].const_[file[file_count].c_count++]);
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
		printf("%s\n", buffer);
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