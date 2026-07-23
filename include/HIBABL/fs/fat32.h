#ifndef HIBABL_FS_FAT32_H
#define HIBABL_FS_FAT32_H

#include <HIBABL/types.h>

typedef struct fat32
{
    u8 sectors_per_cluster;
    u8 numFATs;
    u16 bytes_per_sector;
    u16 reserved_sectors;
    u32 sectors_of_volume;
    u32 parition_addr;
    u32 sectors_per_FAT;
    u32 root_dir_cluster;
    u32 first_data_sector;
    u32 entries_per_sector;
    u32 fat_location;

} fat32 HIBABL_PACKED;

struct fat32_bpb
{
    /* Jump instruction */
    u8  jump_boot[3];

    /* OEM name */
    char     oem_name[8];

    /* BIOS Parameter Block */
    u16 bytes_per_sector;
    u8  sectors_per_cluster;
    u16 reserved_sector_count;
    u8  num_fats;
    u16 root_entry_count;
    u16 total_sectors_16;
    u8  media;
    u16 fat_size_16;
    u16 sectors_per_track;
    u16 num_heads;
    u32 hidden_sectors;
    u32 total_sectors_32;

    /* FAT32 Extended BPB */
    u32 fat_size_32;
    u16 ext_flags;
    u16 fs_version;
    u32 root_cluster;
    u16 fs_info;
    u16 backup_boot_sector;
    u8  reserved[12];

    u8  drive_number;
    u8  reserved1;
    u8  boot_signature;
    u32 volume_id;
    char     volume_label[11];
    char     fs_type[8];

    /* Boot code */
    u8  boot_code[420];

    /* Signature (0xAA55) */
    u16 boot_sector_signature;

} HIBABL_PACKED;

struct fat32_dir_entry
{
    char     name[8];
    char     ext[3];

    u8  attr;
    u8  reserved;
    u8  creation_tenths;
    
    u16 creation_time;
    u16 creation_date;
    u16 access_date;
    u16 cluster_high;
    u16 write_time;
    u16 write_date;
    u16 cluster_low;
    
    u32 size;
} HIBABL_PACKED;

struct fat32_file
{
    u32 first_cluster;
    u32 file_size;
} HIBABL_PACKED;

void fat32_mount(struct fat32* info, u32 partition_lba, u32* fat_table);
u32 fat32_cluster_to_lba(struct fat32* info, u32 cluster);
u32 fat32_next_cluster(u32 cluster, u32* fat_table);
void fat32_read_cluster(struct fat32* fs, void* addr, u32 cluster);
int fat32_find(struct fat32* fs, u32 directory_cluster, const char* name, struct fat32_dir_entry* entry, u32* fat_table);
int fat32_open(struct fat32* fs, const char* name, struct fat32_file* file, u32* fat_table);
void fat32_read(struct fat32* fs, struct fat32_file* file, void* buffer, u32* fat_table, u32 bytes);
int fat32_main(struct fat32* fs, u32* fat_table, const char* filename, void* address, u32 bytes, u32 parition_lba);
void fat32_free(struct fat32* fs, u32* fat_table);
#endif