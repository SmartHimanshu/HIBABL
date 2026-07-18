#ifndef HIBABL_DISK_CHS_H
#define HIBABL_DISK_CHS_H

#include <HIBABL/types.h>


void chs_read_to_disk_buffer(u16 cylinder, u8 head, u8 sector);
void chs_read_to_addr(void* addr, u32 num_sectors, u16 cylinder, u8 head, u8 sector);
extern u8 boot_drive;


#endif