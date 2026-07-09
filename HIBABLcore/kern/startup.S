
.file "startup.S"

#include <HIBABL/symbol.h>
#include <HIBABL/memory.h>

.code16

.globl start
start:
    cli

    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    movl $HIBABL_MEMORY_MACHINE_REAL_STACK, %ebp
    movl %ebp, %esp

    sti

    /* save the boot drive */
	movb	%dl, LOCAL(boot_drive)

	/* reset disk system (%ah = 0) */
	int	$0x13

    calll rm_to_pm

    .code32

    cld

    call hibabl_a20

    movw $0x0F41, 0xB8000

die:
    jmp die

#include "modeswitch.S"


/* Simple A20 gate activation for now. */
hibabl_a20:
    inb $0x92, %al
    orb %al, 2
    outb %al, $0x92
    ret

LOCAL(boot_drive):
    .byte 0