#include <HIBABL/console.h>
#include <HIBABL/int.h>

void __attribute__ ((noreturn))
kmain()
{
    struct bios_regs regs = {0};

    regs.eax = (0x0E << 8) | 'A';  // AH=0Eh, AL='A'
    regs.ebx = 0;                  // BH=0, BL=0

    bios_interrupt(0x10, &regs);


    while(1) { };
}