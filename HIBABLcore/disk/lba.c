#include <HIBABL/disk/lba.h>
#include <HIBABL/types.h>
#include <HIBABL/machine/boot.h>
#include <HIBABL/machine/int.h>
#include <HIBABL/libc.h>
#include <HIBABL/terminal/terminal.h>

void lba_read_to_disk_buffer(u64 lba_addr, u16 transfer_segment, u16 transfer_offset, u16 number_of_sectors)
{
    struct lba_dap DAP;
    DAP.always_0 = 0;
    DAP.lba_addr = lba_addr;
    DAP.num_sectors = number_of_sectors;
    DAP.size_of_dap = 0x10;
    DAP.transfer_offset = transfer_offset;
    DAP.transfer_segment = transfer_segment;
    
    struct bios_regs regs = {0};
    regs.esi = (u32)(&DAP)&0xF;
    regs.ds = ((u32)(&DAP)>>4)&0xFFFF;
    regs.edx = boot_drive;
    regs.eax = 0x4200;
    regs.flags = DEFAULT_INT_FLAGS;

    bios_interrupt(0x13, &regs);

    if(regs.flags&1)
    {
        panic("Error! Disk read failed!");
    }
}

void lba_read_to_addr(void* addr, u32 num_sectors, u64 lba_addr)
{
    void* disk_buffer_addr = (void*)(HIBABL_MACHINE_DISK_BUFFER_ADDR);
    for(u32 i = 0; i < num_sectors; i++)
    {
        lba_read_to_disk_buffer(lba_addr+i, HIBABL_MACHINE_DISK_BUFFER_SEG, 0, 1);
        memcpy((void*)((u32)addr+(i*512)), disk_buffer_addr, 512);
    }
}

void lba_fast_read_to_addr(void* addr, u32 num_sectors, u64 lba_addr, u32 bytes_per_sector)
{
    u32 bios_cycles = num_sectors/127;
    u16 remaining_sectors = num_sectors%127;
    u32 current_address = (u32)addr;
    for(u32 i = 0; i < bios_cycles; i++)
    {
        current_address = (u32)addr+(127*bytes_per_sector*i);
        panic("Error! Disk read failed! %x %x %x", lba_addr+(i*127), current_address>>4, current_address&0xF);
        lba_read_to_disk_buffer(lba_addr+(i*127), current_address>>4, current_address&0xF, 127);
    }
    if(remaining_sectors)
    {
        current_address = (u32)addr + (bios_cycles * 127 * bytes_per_sector);   
        lba_read_to_disk_buffer(lba_addr+(bios_cycles*127), current_address>>4, current_address&0xF, remaining_sectors);
    }
}