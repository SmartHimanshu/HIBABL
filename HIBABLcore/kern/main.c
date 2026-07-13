#include <HIBABL/console.h>
#include <HIBABL/int.h>
#include <HIBABL/mmap.h>
#include <HIBABL/mm.h>

void __attribute__ ((noreturn))
kmain()
{
    console_init();
    console_print(console_make_color(VGA_WHITE, VGA_BLACK), "Welcome to HIBA bootloader!");
    mmap_iterate();
    pmm_init();
    void* ptr = pmm_alloc_pages(4096);
    if(ptr)
    {
        
    }
    while(1) { };
}