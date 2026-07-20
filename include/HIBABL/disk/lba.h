#ifndef HIBABL_DISK_LBA_H
#define HIBABL_DISK_LBA_H

#include <HIBABL/types.h>

struct lba_dap
{
    u8 size_of_dap;
    u8 always_0;
    u16 num_sectors;
    u16 transfer_offset;
    u16 transfer_segment;
    u64 lba_addr;
}HIBABL_PACKED;

extern u8 boot_drive;
void lba_read_to_addr(void* addr, u32 num_sectors, u64 lba_addr);
void lba_fast_read_to_addr(void* addr, u32 num_sectors, u64 lba_addr, u32 bytes_per_sector);

#endif