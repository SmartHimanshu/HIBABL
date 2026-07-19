#include <HIBABL/disk/disk.h>
#include <HIBABL/fs/fat32.h>
#include <HIBABL/memory/mm.h>

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
    dfree(buffer);
}