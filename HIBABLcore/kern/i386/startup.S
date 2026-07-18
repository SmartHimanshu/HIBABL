
.file "startup.S"

#include <HIBABL/machine/symbol.h>
#include <HIBABL/machine/memory.h>

.code16

.extern kmain

.globl start
start:

    /*Save the addressing mode*/
    movb 16(%si), %al
    movb %al, addressing_mode

    cli

    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    movl $HIBABL_MEMORY_MACHINE_REAL_STACK, %ebp
    movl %ebp, %esp

    sti

    

    /* save the boot drive */
	movb	%dl, boot_drive


	/* reset disk system (%ah = 0) */
	int	$0x13

    calll rm_to_pm

    .code32

    cld

    call hibabl_a20

    jmp kmain
die:
    jmp die

#include "modeswitch.S"



/* Simple A20 gate activation for now. */
hibabl_a20:
    inb $0x92, %al
    orb $2, %al
    outb %al, $0x92
    ret

    .globl boot_drive
boot_drive:
    .byte 0

    .globl addressing_mode
addressing_mode:
    .byte 0