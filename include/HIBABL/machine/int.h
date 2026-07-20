#ifndef HIBABL_MACHINE_BIOS_H
#define HIBABL_MACHINE_BIOS_H

#include <HIBABL/types.h>

struct bios_regs
{
    u32 eax;
    u16 es;
    u16 ds;
    u16 flags;
    u16 dummy;
    u32 ebx;
    u32 ecx;
    u32 edi;
    u32 esi;
    u32 edx;
};

#define DEFAULT_INT_FLAGS 0

#define CF_FLAG 1


void __attribute__((regparm(3))) bios_interrupt(u8 intno,
                                                  struct bios_regs *regs);

#endif