#include "./include/linker.h"

Symbol	symbol_buffer	[MAX_ASM_LABEL_TABLE*4];
u64	symbol_count = 0;

Opcode*	hex_buffer;
u64	hexb_count = 0;
// u64	hexb_size = 0;

File*	file;
u32	file_count = 0;

Relocate	relocate	[MAX_ASM_LABEL_TABLE/2];
u64			relocate_count;

s32
main(s32 argc, s8* argv[])
{
	linker(argc, argv);
	return 0;
}
