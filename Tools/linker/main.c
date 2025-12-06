#include "./include/linker.h"

Symbol	symbol_buffer	[MAX_ASM_LABEL_TABLE*4];
u64	symbol_count = 0;

Opcode	hex_buffer	[JARCH_MEMORY_SIZE];
u64	hexb_count = 0;

File	file		[5];
u32	file_count = 0;

Relocate	relocate	[MAX_ASM_LABEL_TABLE/2];
u64			relocate_count;

s32
main(s32 argc, s8* argv[])
{
	linker(argc, argv);
	return 0;
}
