#include <HIBABL/fs/vfs.h>
#include <HIBABL/fs/fat32.h>
#include <HIBABL/machine/offset.h>
#include <HIBABL/libc.h>
#include <HIBABL/memory/mm.h>
#include <HIBABL/system/string.h>
#include <HIBABL/terminal/terminal.h>

struct parition_entry* table_addr = (void*)HIBABL_MACHINE_PARTITION_TABLE_ADDR;

void fat32_parse_path(const char* filename, struct file_path* path)
{
    path = kmalloc(sizeof(file_path));
    
    char** dummy_filename;
    u32* path_height;
    splitsn(dummy_filename, filename, '/', 8, path_height);
    for(u32 i = 0; i < path_height; i++)
    {
        path->name = kmalloc(9);
        for(u8 j = 0; j < 8; j++)
        {
        }
    }
    
}

void vfs_main(const char* filename, void* address, u32 bytes, u8 fs)
{
    struct fat32* fss;
    u32* fat_table;
    fat32_main(fss, fat_table, filename, address, bytes, table_addr[fs].starting_lba);
}