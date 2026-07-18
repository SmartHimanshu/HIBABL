#include <HIBABL/terminal/terminal.h>
#include <HIBABL/disk/disk.h>
#include <HIBABL/machine/int.h>
#include <HIBABL/memory/mmap.h>
#include <HIBABL/memory/mm.h>

void __attribute__ ((noreturn))
kmain()
{
    terminal_init();
    printk("Welcome to HIBA bootloader!\n");
    allocator_init();
    disk_geometry_init();
    void* ptr = dmalloc(512);
    disk_read(ptr, 1, 0);
    dfree(ptr);
    while(1) { };
}