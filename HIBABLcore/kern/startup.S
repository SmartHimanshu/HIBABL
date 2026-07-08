#include "modeswitch.S"

start:
    



/* Simple A20 gate activation for now. */
hibabl_a20:
    inb al, 0x92
    orb al, 2
    outb 0x92, al
    ret