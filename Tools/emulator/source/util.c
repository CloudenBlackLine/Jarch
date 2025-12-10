/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#include "../include/util.h"

void
readfile(s8* f)
{
    FILE* file = fopen(f, "r");
    if(!file) return;
    s32 c;
    u32 count = 0;
    u64 addr = 0;
    s8 buffer[64];
    while((c = fgetc(file)) != EOF)
    {
        if(c == ':')
        {
            count = 0;
            addr = strtoull(buffer, NULL, 16);
            continue;
        }
        else if(c == ';')
        {
            count = 0;
            mem[addr/0x8] = strtoull(buffer, NULL, 16);
            addr = 0;
            continue;
        }
        else if(c == ' ' || c == '\n' || c == '\r') continue;
        buffer[count++] = (char)c;
    }
    fclose(file);
}

void
set_key(struct termios* old_termios)
{
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, old_termios);
    new_termios = *old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void
reset_key(struct termios* old_termios)
{
    tcsetattr(STDIN_FILENO, TCSANOW, old_termios);
}

void
writefile(s8* filename)
{
    return;
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
