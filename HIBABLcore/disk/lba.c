#include <HIBABL/disk/lba.h>
#include <HIBABL/types.h>
#include <HIBABL/machine/boot.h>
#include <HIBABL/machine/int.h>
#include <HIBABL/libc.h>
#include <HIBABL/terminal/terminal.h>

void* disk_buffer_addr = (void*)(HIBABL_MACHINE_DISK_BUFFER_ADDR);

void lba_read_to_disk_buffer(u64 lba_addr, u32 num_sectors)
{
    u32 sectors = num_sectors;
    printd("SECTORS=%x\n", sectors);
    struct lba_dap DAP;
    DAP.always_0 = 0;
    DAP.lba_addr = lba_addr;
    DAP.num_sectors = num_sectors;
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

    if(regs.flags & CF_FLAG)
    {
        panic("Disk error! \nlba address: %x \nsector size: %x\n", lba_addr, num_sectors);
    }

}

void lba_read_to_addr(void* addr, u32 num_sectors, u64 lba_addr)
{
    printd("Disk read done with sector count: %x\n", num_sectors%127);
    for(u32 i = 0; i < num_sectors/127; i++)
    {
        lba_read_to_disk_buffer(lba_addr + (i * 127), 127);
        memcpy((void*)((u32)addr+(i*512*127)), disk_buffer_addr, 512*127);
    }
    if(num_sectors%127)
    {
        lba_read_to_disk_buffer(lba_addr + (num_sectors / 127) * 127, num_sectors%127);
        u32 full_chunks = num_sectors / 127;    
        memcpy((void*)((u32)addr + full_chunks * 127 * 512),disk_buffer_addr,(num_sectors % 127) * 512);
    }
}