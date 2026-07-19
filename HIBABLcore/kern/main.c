#include <HIBABL/terminal/terminal.h>
#include <HIBABL/machine/int.h>
#include <HIBABL/memory/mmap.h>
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
    void* ptr = dmalloc(512);
    dfree(ptr);
    while(1) { };
}