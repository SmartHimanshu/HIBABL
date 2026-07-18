#include <HIBABL/machine/int.h>
#include <HIBABL/disk/chs.h>
#include <HIBABL/types.h>
#include <HIBABL/machine/boot.h>
#include <HIBABL/libc.h>

void chs_read_to_disk_buffer(u16 cylinder, u8 head, u8 sector)
{
    struct bios_regs regs = {0};
    u8 ch = cylinder&0xFF;
    u8 cl = ((cylinder>>2)&0xC0)|sector;
    regs.ecx = (ch<<8)|cl;
    regs.edx = (head<<8) | boot_drive;
    regs.eax = 0x0201;
    regs.es = HIBABL_MACHINE_DISK_BUFFER_SEG;
    regs.flags = DEFAULT_INT_FLAGS;

    bios_interrupt(0x13, &regs);
}

void chs_read_to_addr(void* addr, u32 num_sectors, u16 cylinder, u8 head, u8 sector)
{
    void* disk_buffer_addr = (void*)(HIBABL_MACHINE_DISK_BUFFER_ADDR);
    for(u32 i = 0; i < num_sectors; i++)
    {
        chs_read_to_disk_buffer(cylinder, head, sector);
        memcpy((void*)((u32)addr+(i*512)), disk_buffer_addr, 512);
    }
}

