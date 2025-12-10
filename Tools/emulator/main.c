#include "./include/emulator.h"

u64          mem             [JARCH_MEMORY_SIZE];

u64          system_timer;

u64          reg             [MAX_REGISTER];
u64          SP = 0x0a;
u64          PC = 0x0b;
u64          LR = 0x0c;


s32
main(s32 argc, s8* argv[])
{
    emulator(argc, argv);
    return 0;
}
