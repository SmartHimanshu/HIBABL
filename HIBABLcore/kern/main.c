#include <HIBABL/console.h>


void __attribute__ ((noreturn))
kmain()
{
    console_init();
    console_print(console_make_color(VGA_WHITE, VGA_BLACK), "Hello world!");
    while(1) { };
}