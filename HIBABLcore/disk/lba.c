#include <HIBABL/disk/lba.h>
#include <HIBABL/types.h>
#include <HIBABL/machine/boot.h>
#include <HIBABL/machine/int.h>
#include <HIBABL/libc.h>

void lba_read_to_disk_buffer(u64 lba_addr)
{
    struct lba_dap DAP;
    DAP.always_0 = 0;
    DAP.lba_addr = lba_addr;
    DAP.num_sectors = 1;
    DAP.size_of_dap = 0x10;
    DAP.transfer_offset = 0;
    DAP.transfer_segment = HIBABL_MACHINE_DISK_BUFFER_SEG;
    
    struct bios_regs regs = {0};
    regs.esi = (u32)(&DAP)&0xF;
    regs.ds = ((u32)(&DAP)>>4)&0xFFFF;
    regs.edx = boot_drive;
    regs.eax = 0x4200;
    regs.flags = DEFAULT_INT_FLAGS;

    bios_interrupt(0x13, &regs);

}

void lba_read_to_addr(void* addr, u32 num_sectors, u64 lba_addr)
{
    void* disk_buffer_addr = (void*)(HIBABL_MACHINE_DISK_BUFFER_ADDR);
    for(u32 i = 0; i < num_sectors; i++)
    {
        lba_read_to_disk_buffer(lba_addr);
        memcpy((void*)((u32)addr+(i*512)), disk_buffer_addr, 512);
    }
}