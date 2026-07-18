.file "modeswitch.S"

#include <HIBABL/machine/memory.h>
#include <HIBABL/machine/symbol.h>

/*
+----------------------------------------------------------------------------------+
|                             GLOBAL DESCRIPTOR TABLE                              |
+-------+------------+---------------------+---------------------------------------+
| Index | Selector   | Type                | Description                           |
+-------+------------+---------------------+---------------------------------------+
|   0   | 0x00       | Null                | Mandatory null descriptor             |
|   1   | 0x08       | PM Code             | Ring 0, 32-bit protected mode code    |
|   2   | 0x10       | PM Data             | Ring 0, 32-bit protected mode data    |
|   3   | 0x18       | RM Code             | 16-bit real mode compatible code seg  |
|   4   | 0x20       | RM Data             | 16-bit real mode compatible data seg  |
+-------+------------+---------------------+---------------------------------------+

Descriptor Layout (8 bytes)

 63                            56 55 52 51 48 47                 40
+--------------------------------+-----+-----+--------------------+
|         Base[31:24]            |Flags|Limit|      Access        |
+--------------------------------+-----+-----+--------------------+
 39                            32 31                             16
+--------------------------------+--------------------------------+
|         Base[23:16]            |          Base[15:0]            |
+--------------------------------+--------------------------------+
 15                                                               0
+-----------------------------------------------------------------+
|                     Segment Limit[15:0]                         |
+-----------------------------------------------------------------+

Selectors
---------
  Protected Mode
      CS = 0x08
      DS = ES = FS = GS = SS = 0x10

  Real Mode / 16-bit Segments
      CS = 0x18
      DS = ES = FS = GS = SS = 0x20

Memory Model
------------
  PM Code:
      Base  = 0x00000000
      Limit = 0xFFFFFFFF

  PM Data:
      Base  = 0x00000000
      Limit = 0xFFFFFFFF

  RM Code:
      Base  = 0x00000000
      Limit = 0x0000FFFF
      D/B   = 0 (16-bit)

  RM Data:
      Base  = 0x00000000
      Limit = 0x0000FFFF
      D/B   = 0 (16-bit)
*/

/*
    +------------------------------------------------------------------------------------------+
    | NOTE: We are using 0xFFFF Because we want our dear BIOS to not cry after using segments. |
    +------------------------------------------------------------------------------------------+
*/

protstack:
    .long HIBABL_MEMORY_MACHINE_PM_STACK


gdt:
gdt_null:
    .word 0, 0
    .byte 0, 0, 0, 0

gdt_pm_cs:
    .word	0xFFFF, 0
	.byte	0, 0x9A, 0xCF, 0

gdt_pm_ds:
    .word	0xFFFF, 0
	.byte	0, 0x92, 0xCF, 0

gdt_rm_cs:
    .word	0xFFFF, 0
	.byte	0, 0x9E, 0, 0

gdt_rm_ds:
	.word	0xFFFF, 0
	.byte	0, 0x92, 0, 0

gdtdesc:
    .word	0x27			/* limit */
	.long	gdt			    /* addr */

LOCAL(realidt):
    .word 0x400
    .long 0
pmidt:
    .word 0
    .long 0
.globl rm_to_pm
rm_to_pm:
    .code16
    cli

    xorw %ax, %ax
    movw %ax, %ds

    lgdtl	gdtdesc

    movl %cr0, %eax
    orl $HIBABL_MEMORY_CPU_CR0_PE_ON, %eax
    movl %eax, %cr0

    ljmpl $HIBABL_MEMORY_MACHINE_PM_CS, $pmcseg

    .code32
pmcseg:
    movw $HIBABL_MEMORY_MACHINE_PM_DS, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %gs
    movw %ax, %fs

    /* 
    We use the real stack as a scratch space for now.
    We store the return address currently on the stack to here.
    */
    movl (%esp), %eax
    movl %eax, HIBABL_MEMORY_MACHINE_REAL_STACK

    movl protstack, %eax
    movl %eax, %esp
    movl %eax, %ebp

    /* We restore the return address */

    movl HIBABL_MEMORY_MACHINE_REAL_STACK, %eax
    movl %eax, (%esp)

    sidt LOCAL(realidt)
    lidt pmidt

    ret
.globl pm_to_rm
pm_to_rm:
    /* We want to be absolutely sure that we have a gdt loaded in. */
    lgdt gdtdesc

    sidt pmidt
    lidt LOCAL(realidt)

    /* Save the prot stack */
    movl %esp, %eax
    movl %eax, protstack

    /* Get the return address */
    movl (%esp), %eax
    movl %eax, HIBABL_MEMORY_MACHINE_REAL_STACK
    /* Using the real stack as a scratch space again */

    movl $HIBABL_MEMORY_MACHINE_REAL_STACK, %eax
    movl %eax, %esp
    movl %eax, %ebp

    movw $HIBABL_MEMORY_MACHINE_RM_DS, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    ljmp  $HIBABL_MEMORY_MACHINE_RM_CS, $tmpcseg
tmpcseg:
    .code16gcc

    /* set the mode to real mode */
    movl %cr0, %eax
    andl $(~HIBABL_MEMORY_CPU_CR0_PE_ON), %eax
    movl %eax, %cr0

    ljmpl $0, $realseg
realseg:

    xorl %eax, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    sti

    retl