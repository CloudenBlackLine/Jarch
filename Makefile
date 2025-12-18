local:
	gcc -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -ITools/assembler/include Tools/assembler/source/*.c Tools/assembler/main.c -o bin/asm
	gcc -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -ITools/linker/include Tools/linker/source/*.c Tools/linker/main.c -o bin/lnk
	gcc -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -ITools/emulator/include Tools/emulator/source/*.c Tools/emulator/main.c -o bin/emu
global:
	sudo gcc -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -ITools/assembler/include Tools/assembler/source/*.c Tools/assembler/main.c -o /bin/asm
	sudo gcc -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -ITools/linker/include Tools/linker/source/*.c Tools/linker/main.c -o /bin/lnk
	sudo gcc -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -ITools/emulator/include Tools/emulator/source/*.c Tools/emulator/main.c -o /bin/emu
