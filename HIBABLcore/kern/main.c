#include <HIBABL/console.h>
#include <HIBABL/int.h>
#include <HIBABL/mmap.h>
#include <HIBABL/mm.h>

void __attribute__ ((noreturn))
kmain()
{
    console_init();
    console_print(console_make_color(VGA_WHITE, VGA_BLACK), "Welcome to HIBA bootloader!\n");
    mmap_iterate();
    pmm_init();
    heap_init();
    while(1) { };
}