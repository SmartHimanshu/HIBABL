#include <HIBABL/terminal/terminal.h>
#include <HIBABL/disk/lba.h>
#include <HIBABL/machine/int.h>
#include <HIBABL/memory/mmap.h>
#include <HIBABL/memory/mm.h>

void __attribute__ ((noreturn))
kmain()
{
    terminal_init();
    printk("Welcome to HIBA bootloader!\n");
    allocator_init();
    void* ptr = dmalloc(512);
    lba_read_to_addr((void*)0x00, 1, 0);
    dfree(ptr);
    while(1) { };
}