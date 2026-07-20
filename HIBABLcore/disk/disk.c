#include <HIBABL/disk/chs.h>
#include <HIBABL/disk/lba.h>
#include <HIBABL/disk/disk.h>
#include <HIBABL/types.h>
#include <HIBABL/machine/int.h>

u8 heads_per_cylinder;
u8 sectors_per_track;
u16 max_cylinders;

static void get_disk_geometry(u8 *heads_per_cylinder, u8 *sectors_per_track, u16 *max_cylinders)
{
    struct bios_regs regs = {0};
    regs.flags = DEFAULT_INT_FLAGS;
    regs.eax = 0x0800;
    regs.edx = boot_drive;
    
    bios_interrupt(0x13, &regs);

    *heads_per_cylinder = ((regs.edx&0xFF00)>>8)+1;
    *sectors_per_track = regs.ecx & 0x3F;
    *max_cylinders = ((regs.ecx>>8)&0xFF) | (regs.ecx&0xC0)<<2;
}

void disk_geometry_init(void)
{
    get_disk_geometry(&heads_per_cylinder, &sectors_per_track, &max_cylinders);
}

static void convert_lba_to_chs(u16 *cylinder, u8 *head, u8 *sector, u32 lba_addr)
{
    *cylinder = lba_addr / (heads_per_cylinder * sectors_per_track);
    u32 temp = lba_addr % (heads_per_cylinder * sectors_per_track);
    *head = temp / sectors_per_track;
    *sector = (temp % sectors_per_track)+1;
};

void disk_read(void* addr, u32 num_sectors, u64 address, u32 bytes_per_sector)
{
    if(!bytes_per_sector)
    {
        if(addressing_mode)
        {
            lba_read_to_addr(addr, num_sectors, address);
        }
        else
        {
            u16 cylinder;
            u8 head;
            u8 sector;
            convert_lba_to_chs(&cylinder, &head, &sector, (u32)address);
            chs_read_to_addr(addr, num_sectors, cylinder, head, sector);
        }
    }
    else
    {
        lba_fast_read_to_addr(addr, num_sectors, address, bytes_per_sector);
    }
}