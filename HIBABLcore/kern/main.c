#include <HIBABL/terminal/terminal.h>
#include <HIBABL/memory/mm.h>
#include <HIBABL/disk/disk.h>
#include <HIBABL/fs/fat32.h>

void __attribute__ ((noreturn))
kmain()
{
    terminal_init();
    printk("Welcome to HIBA bootloader!\n");
    allocator_init();
    disk_geometry_init();
    struct fat32 thing = {0};
    fat32_mount(&thing, 2048);
    u32* fat_table = dmalloc(thing.sectors_per_FAT*thing.bytes_per_sector);
    disk_read(fat_table, thing.sectors_per_FAT, thing.fat_location);
    struct fat32_dir_entry e = {0};
    printk("Have we found the thing? : %d", fat32_find(&thing, 2, "CORE    BIN", &e, fat_table));
    while(1) { };
}