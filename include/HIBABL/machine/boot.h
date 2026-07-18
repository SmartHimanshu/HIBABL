#ifndef HIBABL_CORE_BOOT_H
#define HIBABL_CORE_BOOT_H

#include <HIBABL/machine/offset.h>

/*

*/
#define HIBABL_MACHINE_CORE_SIZE_PARAMS 0xA
/*
The segment of the disk buffer. Since different platforms use different sector sizes, we need
a safe place. Disk buffer is 32k long and at a 64k boundary to prevent crashes.
*/
#define HIBABL_MACHINE_DISK_BUFFER_SEG 0x7000
#define HIBABL_MACHINE_DISK_BUFFER_ADDR HIBABL_MACHINE_DISK_BUFFER_SEG<<4
/*
The stack segment for our beautiful stack.
*/
#define HIBABL_MACHINE_STACK_SEG 0x2000
/*
The offset at which The BPB ends for most systems. 
*/
#define HIBABL_MACHINE_BOOT_BPB_END 0x5a
/*
The offset at which to place the boot signature.
*/
#define HIBABL_MACHINE_BOOT_END 510
/*
The Boot signature checked by the bios which makes sure that our boot.bin correctly
loads into the address 0x7c00.
*/
#define HIBABL_MACHINE_BOOT_SIGNATURE 0xaa55


#endif