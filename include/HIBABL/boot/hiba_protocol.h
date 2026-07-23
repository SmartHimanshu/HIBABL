#ifndef HIBABL_HIBA_PROTOCOL_H
#define HIBABL_HIBA_PROTOCOL_H

#include <HIBABL/types.h>
#include <HIBABL/memory/mmap.h>

struct vbe_info
{
    char signature[4];
    u16 version;

    u32 oem_string;
    u32 capabilities;
    u32 video_mode_ptr;

    u16 total_memory;

    u16 oem_software_rev;
    u32 oem_vendor_name_ptr;
    u32 oem_product_name_ptr;
    u32 oem_product_rev_ptr;

    u8 reserved[222];
    u8 oem_data[256];
} HIBABL_PACKED;

struct vbe_mode_info
{
    /* 0x00 */
    u16 ModeAttributes;

    u8  WinAAttributes;
    u8  WinBAttributes;
    u16 WinGranularity;
    u16 WinSize;
    u16 WinASegment;
    u16 WinBSegment;
    u32 WinFuncPtr;

    u16 BytesPerScanLine;

    /* 0x12 */
    u16 XResolution;
    u16 YResolution;

    u8  XCharSize;
    u8  YCharSize;
    u8  NumberOfPlanes;
    u8  BitsPerPixel;
    u8  NumberOfBanks;
    u8  MemoryModel;
    u8  BankSize;
    u8  NumberOfImagePages;
    u8  Reserved1;

    /* Direct color fields */
    u8  RedMaskSize;
    u8  RedFieldPosition;

    u8  GreenMaskSize;
    u8  GreenFieldPosition;

    u8  BlueMaskSize;
    u8  BlueFieldPosition;

    u8  RsvdMaskSize;
    u8  RsvdFieldPosition;

    u8  DirectColorModeInfo;

    /* Linear framebuffer info */
    u32 PhysBasePtr;

    u32 OffScreenMemOffset;
    u16 OffScreenMemSize;

    /* VBE 2.0+ */
    u16 LinBytesPerScanLine;

    u8  BnkNumberOfImagePages;
    u8  LinNumberOfImagePages;

    u8  LinRedMaskSize;
    u8  LinRedFieldPosition;

    u8  LinGreenMaskSize;
    u8  LinGreenFieldPosition;

    u8  LinBlueMaskSize;
    u8  LinBlueFieldPosition;

    u8  LinRsvdMaskSize;
    u8  LinRsvdFieldPosition;

    u32 MaxPixelClock;

    u8  Reserved2[190];
} HIBABL_PACKED;

struct hibaboot_header
{
    u8 jump[3];
    u8 rsved[5];
    u64 header_magic;
    u64 flags;
    u8 code[488];

} HIBABL_PACKED;


struct boot_info
{
    u64 magic;
    u64 version;
    struct screen_info* si;
    struct e820_mm* mm;

} HIBABL_PACKED;

struct screen_info
{
    u8 prev_video_type;     /* We are dealing with serial vga or framebuffer */
    u16 prev_video_points;  /* Height of a character in 8xG format. */
    u16 prev_video_page;    /* The page of text. Usually 0 */
    u8 prev_cursor_x;       /* Previous cursor position HIBABL will leave */
    u8 prev_cursor_y;       
    u8 prev_video_cols;     /* Text size dimensions. (80x25 text etc.)*/     
    u8 prev_video_rows;     

    /* Data in case we encounter most certainly framebuffer support */
    /* VESA graphic mode. */

    /* Physical info. */
    u16 fb_width;
    u16 fb_height;
    u16 fb_depth;

    u32 fb_base;
    u32 fb_size;

    u8 red_size;
    u8 red_pos;
    
    u8 blue_size;
    u8 blue_pos;
    
    u8 green_size;
    u8 green_pos;
    
    u8 rsvd_size;
    u8 rsvd_pos;

    u16 fb_pitch;       /* Bytes per row. */

} HIBABL_PACKED;

int load_hibaos(const char* filename, struct boot_info* boot_info);

#define HIBABOOT_HEADER_MAGIC 'HIBABOOT'

#define HIBABOOT_HEADER_E820 1<<0
#define HIBABOOT_HEADER_SI 1<<1
#define HIBABOOT_HEADER_ACPI 1<<2

#define BOOTINFO_HEADER_MAGIC 'BOOTINFO'

#define BOOTINFO_SCREEN_INFO_NONE 0
#define BOOTINFO_SCREEN_INFO_FRAMEBUFFER 1
#define BOOTINFO_SCREEN_INFO_VGA 2

#endif