#include <HIBABL/fs/vfs.h>
#include <HIBABL/fs/fat32.h>
#include <HIBABL/machine/offset.h>
#include <HIBABL/libc.h>
#include <HIBABL/memory/mm.h>


struct parition_entry* table_addr = (void*)HIBABL_MACHINE_PARTITION_TABLE_ADDR;

void vfs_main(const char* filename, void* address, u32 bytes, u8 fs)
{
    fat32_main(filename, address, bytes, table_addr[fs].starting_lba);
}