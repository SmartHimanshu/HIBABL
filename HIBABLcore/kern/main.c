#include <HIBABL/terminal/terminal.h>
#include <HIBABL/memory/mm.h>
#include <HIBABL/disk/disk.h>
#include <HIBABL/disk/lba.h>
#include <HIBABL/fs/fat32.h>
#include <HIBABL/fs/vfs.h>
#include <HIBABL/boot/hiba_protocol.h>

struct boot_info* kmain()
{
    terminal_init();
    printk("Welcome to HIBA bootloader!\n");
    allocator_init();
    disk_geometry_init();
    struct boot_info* boot_info;
    struct fat32* fs;
    u32* fat_entry;
    lba_read_to_addr((void*)0x400000, 127, 2048);
    printk("Before fat32");
    int res = fat32_main(fs, fat_entry, "KERNEL  BIN", (void*)0x400000, 1024, 2048);
    printk("Have we succeded in loading the kernel? : %d\n", res);
    return boot_info;
    
}