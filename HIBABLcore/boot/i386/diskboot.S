.file "diskboot.S"

#include <HIBABL/machine/boot.h>
#include <HIBABL/machine/symbol.h>
#include <HIBABL/machine/offset.h>

#define MSG(x) movw $x, %si; call LOCAL(message)

.text

.code16

.globl start, _start;
_start:
start:
    /* Save drive reference */
    pushw %dx

    pushw %si
    MSG(stage2_welcome_msg)
    popw %si

    movw $LOCAL(core_size_params), %di

    /* Check the mode. CHS is zero, LBA if non-zero */
    cmpb    $0, 16(%si)

    je LOCAL(chs_addressing)

    /*
    Till here, the disk_address_packet stored in %si as a valid dap,
    Therefore, no need to impose dap size, reserved size, Transfer buffer segment and offset.
    As we are pointing in memory to Disk buffer segment.
    And the number of sectors to transfer. (since we are transferring 1 sector at a time).
    The only thing that needs to change is the LBA address.
    */

    movl (%di), %eax    /* load LBA start */
    movl %eax, 8(%si)
    movl 4(%di), %eax
    movl %eax, 12(%si)


LOCAL(load_loop):

    movb $0x42, %ah     /* Perform the read into disk address buffer. */
    int $0x13

    pusha
    pushw %ds

    movw    $0x100, %cx
    movw    $HIBABL_MACHINE_DISK_BUFFER_SEG, %bx
    movw    %bx, %ds
    
    xorw    %si, %si
    movw    %cs:LOCAL(core_address_pointer), %di
    movw	%si, %es

    cld
    rep movsw

    

    popw %ds
    popa
    
    

    movl 8(%si), %eax   /* Advance the LBA address to read from. */
    addl $1, %eax
    movl %eax, 8(%si)
    movl 12(%si), %eax
    adcl $0, %eax       /* Add with carry */
    movl %eax, 12(%si)

    movw LOCAL(core_address_pointer), %ax  /* Advance the Core Address pointer to next sector */
    addw $0x200, %ax
    movw %ax, LOCAL(core_address_pointer) 

    movw 8(%di), %ax    /* We check for when the loading is complete. */ 
    subw $1, %ax
    movw %ax, 8(%di)    

    cmpw $0, %ax

    

    jne LOCAL(load_loop)

    ljmp $0, $(HIBABL_MACHINE_STAGE2_ADDR+0x200)
LOCAL(chs_addressing):
    pushw %cs
    popw %ds
    MSG(stage2_welcome_msg)




/*
Simple print call for stage 2 messages. 
Preserves ax and bx from destruction.
*/
LOCAL(message):
    pushw %ax
    pushw %bx
    movb $0x0E, %ah
    movb $0, %bh
    movb $0x07, %bl
1:
    lodsb
    cmp $0, %al
    je 2f
    int $0x10
    jmp 1b
2:
    popw %bx
    popw %ax
    ret

stage2_welcome_msg:
    .asciz "Loading HIBABL Alpha Version 2...\r\n"

LOCAL(core_address_pointer):
    .word HIBABL_MACHINE_STAGE2_ADDR+0x200

LOCAL(core_size_params):
    .long 2, 0
    .word 48
.org 0x200