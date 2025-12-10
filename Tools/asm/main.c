/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#include "./include/assembler.h"
#include "./include/util.h"

s8			acode			[MAX_ASM_SYNTAX_BUFFER];
u64			asm_syntax_size = 0;

u64			text_buffer		[MAX_TEXT_BUFFER];
u64			text_offset = 0;

u64			data_buffer		[MAX_DATA_BUFFER];
u64			data_offset = 0;

u64			const_buffer		[MAX_CONST_BUFFER];
u64			const_offset = 0;

Relocate		relocate_buffer		[MAX_ASM_LABEL_TABLE/2];
u64			relocate_offset = 0;



Symbol 			labels			[MAX_ASM_LABEL_TABLE];
u64			symbol_count = 0;



s32
main(s32 argc, s8* argv[])
{
	if(argc < 3) return 1;

	//printf("%s, %c\n%s, %c\n", argv[1], *argv[1]-1, argv[2], (*argv[2]-8));

	//if(*argv[1]-1 != 's') { printf("Error - %s is not a ASSEMBLY supported file. Must be '.s' type file\n", argv[1]); return 1; }
	//if(*argv[2]-1 != 'o') { printf("Error - %s is not a OBJECT file. Must be '.o' type file.\n", argv[2]); return 1; }
	
	asm_syntax_size = readfile(argv[1], acode, MAX_ASM_SYNTAX_BUFFER);
	assembler();
	writefile(argv[2]);
	return 0;
}
