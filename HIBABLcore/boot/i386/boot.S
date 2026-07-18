.file "boot.S"

#include <HIBABL/machine/boot.h>
#include <HIBABL/machine/symbol.h>

#define MSG(x) movw $x, %si; call LOCAL(message)

.code16

.globl start, _start;
_start:
start:
    jmp LOCAL(postBPB)
    nop

.org HIBABL_MACHINE_BOOT_BPB_END
LOCAL(postBPB):

    /*movzbl
    Workaround for buggy BIOSES that do not give 0x80-0x9f (Hard disks).
    Set dl as 0x80 as default in case of buggy BIOS.
    */

    testb $0x80, %dl
    jz 1f
    jmp 2f
1:
    movb $0x80, %dl
2:
    /*
    Some BIOSes send us to address 0x7c0:0 instead of clean 0:0x7c00
    */
    ljmp $0, $LOCAL(reset_start)

LOCAL(reset_start):

    cli
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    movw $HIBABL_MACHINE_STACK_SEG, %sp

    sti
    

    /*
    Boot drive stores the boot drive from which we will load the stage 2 from.
    We are now gonna check whether stage2_boot_drive has changed and if it doesn't, 
    we are gonna go through our original boot drive from which we are loading.
    */

    movb stage2_boot_drive, %al
    cmpb $0x00, %al
    je 1f
    movb %al, %dl
1:
    /* Save drive reference */
    pushw %dx
    /* We can now say we are in */
    MSG(hello)

LOCAL(determine_addressing):
    movw $disk_address_packet, %si

    /* Determine if LBA is supported */
    movb $0x41, %ah
    movw $0x55aa, %bx
    int $0x13

    /*
    %dl might be extremely violated by int 13 ah=41h
    So we just restore it
    */

    popw %dx
    pushw %dx
    
    jc LOCAL(chs_addressing)
    cmpw $0xaa55, %bx
    jne LOCAL(chs_addressing)

    andw $1, %cx
    jz LOCAL(chs_addressing)

LOCAL(lba_addressing):
    xor %ax, %ax
    incw %ax
    movw %ax, 16(%si)
    /* set the mode to lba addressing */

    movw %ax, 2(%si)
    /* set the number of sectors to read to 1 */

    movw $HIBABL_MACHINE_DISK_BUFFER_SEG, 6(%si)
    /* we are setting up the disk buffer */

    movl LOCAL(stage2_lba_low), %eax
    movl %eax, 8(%si)
    movl LOCAL(stage2_lba_high), %eax
    movl %eax, 12(%si)
    /* move the lba address in */

    /*
    BIOS call "INT 0x13 Function 0x42" to read sectors from disk into memory
    Call with	%ah = 0x42
    %dl = drive number
    %ds:%si = segment:offset of disk address packet
    Return %al:0 in sucess and err code if not completed.
    */

    movb $0x42, %ah
    int $0x13

    /* LBA is not available */
    jc LOCAL(chs_addressing)

    /* Prepare for copy */
    movw $HIBABL_MACHINE_DISK_BUFFER_SEG, %bx
    jmp LOCAL(copy)

LOCAL(chs_addressing):
    /* Get the disk geometry from the BIOS */
    movb $8, %ah
    int $0x13

    movzbl %dh, %eax    /* Store the max number of heads */
    movb %ah, 16(%si)   /* Set the addressing mode to chs */

    incw %ax            /* max index -> count */
    movl %eax, 4(%si)   /* save head count */

    movzbw	%cl, %dx    /* %ch: Holds the lower 8 bits of the cylinder number (Bits 0–7) */
	shlw	$2, %dx     /* %cl: Bits 0–5: The maximum sector number. */
	movb	%ch, %al    /* Bits 6–7: The upper 2 bits of the cylinder number (Bits 8–9). */
	movb	%dh, %ah    /* Store the 10 bits of cylinder number in ax */
    
	incw	%ax         /* save number of cylinder count */
	movw	%ax, 8(%si)

    movzbw	%dl, %ax    /* arrange the number of sectors into ax */
	shrb	$2, %al

	movl	%eax, (%si) /* save number of sectors */

	movl	LOCAL(stage2_lba_low), %eax  /* load logical sector start (bottom half) */

    xorl    %ebx, %ebx

    divl    (%si)       /* Divide by number of sectors */

    movb    %dl, %cl    /* Save sector start */

    xorw    %dx, %dx    /* Zero edx */
    divl    4(%si)      /* Divide by number of heads */

    incb    %cl         /* Normalize Sector start to 1 based */
    
	movb	%al, %ch    /* low bits of cylinder start */

	xorb	%al, %al    /* high bits of cylinder start */
	shrw	$2, %ax
	orb	%al, %cl
   
	movb	%dl, %al    /* save head start */
	
	popw	%dx         /* restore %dl */

	movb	%al, %dh    /* head start */

    /*
    BIOS call "INT 0x13 Function 0x2" to read sectors from disk into memory
    Call with	%ah = 0x2
    %al = number of sectors
    %ch = cylinder
    %cl = sector (bits 6-7 are high bits of "cylinder")
    %dh = head
    %dl = drive
    %es:%bx = segment:offset of buffer
    return %al=0 for sucess, err code on failure
    */

    movw $HIBABL_MACHINE_DISK_BUFFER_SEG, %bx   /* We are gonna use disk buffer */
    movw %bx, %es

    

    xorw %bx, %bx   /* set offset of buffer to 0. */
    movw	$0x0201, %ax	/* function 2 */
    int $0x13

    movw	%es, %bx

LOCAL(copy):
    /*
	We need to save %cx and %si because the startup code in
	stage2 uses them without initializing them.
	*/
	pusha
	pushw	%ds

	movw	$0x100, %cx
	movw	%bx, %ds
	xorw	%si, %si
	movw	$HIBABL_MACHINE_STAGE2_ADDR, %di
	movw	%si, %es

	cld

	rep
	movsw

	popw	%ds
	popa


    jmp     HIBABL_MACHINE_STAGE2_ADDR
LOCAL(die):
    cli
    hlt
    jmp LOCAL(die)
/*
Simple print call for stage 1 messages. 
Preserves ax and bx from destruction.
*/
LOCAL(message):
    pushw %ax
    pushw %bx
    movb $0x0E, %ah
    movb $0, %bh
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

hello:
    .asciz "Welcome to HIBABL version Alpha 2\r\n"

/*
In order to save memory,
We use this disk_address_packet to store the disk geometry
and related things into disk_address_packet label
*/

disk_address_packet:
    .byte 0x10  /* Size of disk address packet */
    .byte 0     
    .word 0     /* Number of sectors to transfer */
    .long 0     /* Transfer buffer segment:offset */
    .long 0     /* LBA address low */
    .long 0     /* LBA address high */
addressing_mode:
    .byte 0
stage2_boot_drive:
    .byte 0x00
LOCAL(stage2_lba_low):
    .long 1
LOCAL(stage2_lba_high):
    .long 0


.org HIBABL_MACHINE_BOOT_END
.word HIBABL_MACHINE_BOOT_SIGNATURE