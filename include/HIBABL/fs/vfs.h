#ifndef HIBABL_FS_VFS_H
#define HIBABL_FS_VFS_H


#include <HIBABL/types.h>

struct parition_entry
{
    u8 boot_flag;
    u8 start_chs[3];
    u8 parition_type;
    u8 end_chs[3];
    u32 starting_lba;
    u32 size;
} HIBABL_PACKED;

typedef struct file_path
{
    char* name;
    struct file_path* next;
} file_path HIBABL_PACKED;


void fat32_parse_path(const char* filename, struct file_path* path);

#endif