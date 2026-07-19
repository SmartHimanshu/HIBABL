#include <HIBABL/disk/disk.h>
#include <HIBABL/fs/fat32.h>
#include <HIBABL/memory/mm.h>
#include <HIBABL/system/string.h>
#include <HIBABL/libc.h>

void fat32_mount(struct fat32* info, u32 partition_lba)
{
    void* buffer = dmalloc(512);
    disk_read(buffer, 1, partition_lba);
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
    disk_read(addr, fs->sectors_per_cluster, fat32_cluster_to_lba(fs, cluster));
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
            return 1;
        }
        current_cluster = fat32_next_cluster(current_cluster, fat_table);
        dfree(entries);
    }    
    return 0;
}