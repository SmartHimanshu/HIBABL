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
    printk("Have we succeded? : %d\n", fat32_main("CORE    BIN", (void*)0x400000, 1024, 2048));
    while(1) { };
}