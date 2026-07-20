#include <HIBABL/disk/disk.h>
#include <HIBABL/fs/fat32.h>
#include <HIBABL/memory/mm.h>
#include <HIBABL/system/string.h>
#include <HIBABL/libc.h>
#include <HIBABL/system/error.h>
#include <HIBABL/terminal/terminal.h>

void fat32_mount(struct fat32* info, u32 partition_lba, u32* fat_table)
{
    void* buffer = dmalloc(512);
    disk_read(buffer, 1, partition_lba, 0);
    
    struct fat32_bpb* first_sector = (struct fat32_bpb*)buffer;
    
    info->bytes_per_sector = first_sector->bytes_per_sector;
    info->numFATs = first_sector->num_fats;
    info->parition_addr = partition_lba;
    info->reserved_sectors = first_sector->reserved_sector_count;
    info->root_dir_cluster = first_sector->root_cluster;
    info->sectors_of_volume = first_sector->total_sectors_16 | first_sector->total_sectors_32;
    info->sectors_per_cluster = first_sector->sectors_per_cluster;
    info->sectors_per_FAT = first_sector->fat_size_16 | first_sector->fat_size_32;
    
    info->first_data_sector = partition_lba + info->reserved_sectors + (info->numFATs * info->sectors_per_FAT);
    info->entries_per_sector = info->bytes_per_sector * info->sectors_per_cluster/sizeof(struct fat32_dir_entry);
    info->fat_location = partition_lba + info->reserved_sectors;
    
    dfree(buffer);


    fat_table = dmalloc(info->sectors_per_FAT*info->bytes_per_sector);
    disk_read(fat_table, info->sectors_per_FAT, info->fat_location, info->bytes_per_sector);
}

u32 fat32_cluster_to_lba(struct fat32* info, u32 cluster)
{
    return ((cluster - 2) * info->sectors_per_cluster) + info->first_data_sector;
}

u32 fat32_next_cluster(u32 cluster, u32* fat_table)
{
    return fat_table[cluster] & 0x0FFFFFFF;
}

void fat32_read_cluster(struct fat32* fs, void* addr, u32 cluster)
{
    disk_read(addr, fs->sectors_per_cluster, fat32_cluster_to_lba(fs, cluster), fs->bytes_per_sector);
}


int fat32_find(struct fat32* fs, u32 directory_cluster, const char* name, struct fat32_dir_entry* entry, u32* fat_table)
{
    u32 current_cluster = directory_cluster;
    while(current_cluster<0x0FFFFFF8)
    {
        struct fat32_dir_entry* entries = dmalloc(fs->entries_per_sector*fs->bytes_per_sector);
        fat32_read_cluster(fs, (void*)entries, current_cluster);
        for(int i = 0; i < fs->entries_per_sector; i++)
        {
            if(strncmp(entries[i].name, name, 8))
            {
                continue;
            }
            if(strncmp(entries[i].ext, name+8, 3))
            {
                continue;
            }
            memcpy(entry, &entries[i], sizeof(struct fat32_dir_entry));
            dfree(entries);
            return SUCCESS;
        }
        current_cluster = fat32_next_cluster(current_cluster, fat_table);
        dfree(entries);
    }    
    return ERRNF;
}

int fat32_open(struct fat32* fs, const char* name, struct fat32_file* file, u32* fat_table)
{
    struct fat32_dir_entry entry = {0};
    int res = fat32_find(fs, 2, name, &entry, fat_table);
    file->file_size = entry.size;
    file->first_cluster = (entry.cluster_high<<16)|entry.cluster_low;
    return res;
}

void fat32_read(struct fat32* fs, struct fat32_file* file, void* buffer, u32* fat_table, u32 bytes)
{
    u32 bytes_per_cluster = fs->bytes_per_sector*fs->sectors_per_cluster;
    u32 remaining_bytes = bytes;
    u32 current_cluster = file->first_cluster;
    while(remaining_bytes>0)
    {
        disk_read(buffer, fs->sectors_per_cluster, fat32_cluster_to_lba(fs, current_cluster), fs->bytes_per_sector);
        buffer += bytes_per_cluster;
        remaining_bytes -= bytes_per_cluster;
        current_cluster = fat32_next_cluster(current_cluster, fat_table);
    }
}

//This is gonna be our last function and its sole job will be to load our main kernel into memory 

int fat32_main(const char* filename, void* address, u32 bytes, u32 parition_lba)
{
    struct fat32* fs;
    u32* fat_table;
    struct fat32_file file_data;
    fat32_mount(fs, parition_lba, fat_table);
    int res = fat32_open(fs, filename, &file_data, fat_table);
    fat32_read(fs, &file_data, address, fat_table, bytes);
    return res;
}