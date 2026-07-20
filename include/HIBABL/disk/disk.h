#ifndef HIBABL_DISK_DISK_H
#define HIBABL_DISK_DISK_H

#include <HIBABL/types.h>


void disk_geometry_init(void);
void disk_read(void* addr, u32 num_sectors, u64 address, u32 bytes_per_sector);
extern u8 addressing_mode;


#endif