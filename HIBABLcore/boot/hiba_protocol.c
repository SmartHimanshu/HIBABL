#include <HIBABL/boot/hiba_protocol.h>
#include <HIBABL/machine/offset.h>
#include <HIBABL/fs/fat32.h>
#include <HIBABL/machine/int.h>
#include <HIBABL/terminal/terminal.h>
#include <HIBABL/libc.h>
#include <HIBABL/machine/boot.h>
#include <HIBABL/memory/mm.h>

static void* vbe_farptr_to_ptr(u32 farptr)
{
    u16 offset  = farptr & 0xFFFF;
    u16 segment = farptr >> 16;

    return (void*)(((u32)segment << 4) + offset);
}

int get_vbe_mode_info(struct vbe_mode_info* buffer, u16 mode_number)
{
    struct bios_regs regs = {0};
    
    regs.eax = 0x4F01;

    regs.ecx = mode_number;

    regs.es = 0xFFFF&((u32)buffer>>4);
    regs.edi = 0xF&(u32)buffer;

    bios_interrupt(0x10, &regs);

    if ((regs.eax & 0xFFFF) != 0x004F)
        return 0;

    return 1;
}

int get_into_vbe_mode(u16 mode_number)
{
    struct bios_regs regs = {0};

    regs.flags = DEFAULT_INT_FLAGS;
    regs.eax = 0x4F02;
    regs.ebx = 0x4000 | mode_number;
    
    bios_interrupt(0x10, &regs);

    if ((regs.eax & 0xFFFF) != 0x004F)
    {
        printk("AX=%x BX=%x\n", regs.eax, regs.ebx);
        return 0;
    }
    return 1;
}

int get_vbe_mode(struct vbe_info* buffer)
{
    struct bios_regs regs = {0};
    
    regs.eax = 0x4F00;
    
    memcpy(buffer->signature, "VBE2", 4);
    regs.es = ((u32)buffer>>4)&0xFFFF;
    regs.edi = (u32)buffer&0xF;
    

    bios_interrupt(0x10, &regs);
    
    if((regs.eax&0xFFFF) != 0x004F || memcmp(buffer->signature, "VESA", 4))
        return 0;
    
    return 1;
}

int enumerate_vbe_modes(struct vbe_info* vbe_header, struct vbe_mode_info** best_mode)
{
    struct vbe_mode_info* best_mode_list;
    u16* mode_numbers;

    struct vbe_mode_info* buffer =
        (struct vbe_mode_info*)(HIBABL_MACHINE_DISK_BUFFER_ADDR);

    u16* ptr_to_modes =
        (u16*)vbe_farptr_to_ptr(vbe_header->video_mode_ptr);

    u64 max = 0;

    u32 index = 0;
    u32 number_of_perfect_modes = 8;

    mode_numbers =
        kmalloc(number_of_perfect_modes * sizeof(u16));

    best_mode_list =
        kmalloc(number_of_perfect_modes *
                sizeof(struct vbe_mode_info));

    for(u32 i = 0; ptr_to_modes[i] != 0xFFFF; i++)
    {
        if(!get_vbe_mode_info(buffer, ptr_to_modes[i]))
        {
            continue;
        }

        if(!(buffer->ModeAttributes & (1 << 0))) continue;
        if(!(buffer->ModeAttributes & (1 << 4))) continue;
        if(!(buffer->ModeAttributes & (1 << 7))) continue;

        if(buffer->MemoryModel != 6) continue;
        if(buffer->BitsPerPixel < 24) continue;
        if(buffer->PhysBasePtr == 0) continue;
        if(buffer->XResolution < 640) continue;
        if(buffer->YResolution < 480) continue;

        u64 score =
            (u64)buffer->XResolution *
            (u64)buffer->YResolution *
            (u64)buffer->BitsPerPixel;

        if(score > max)
        {
            if(index >= number_of_perfect_modes)
            {
                number_of_perfect_modes *= 2;

                mode_numbers =
                    krealloc(mode_numbers,
                             number_of_perfect_modes *
                             sizeof(u16));

                best_mode_list =
                    krealloc(best_mode_list,
                             number_of_perfect_modes *
                             sizeof(struct vbe_mode_info));
            }

            mode_numbers[index] = ptr_to_modes[i];

            memcpy(
                &best_mode_list[index],
                buffer,
                sizeof(struct vbe_mode_info)
            );

            max = score;
            index++;
        }
    }
    for(int i = index - 1; i >= 0; i--)
    {
        if(get_into_vbe_mode(mode_numbers[i]))
        {
            *best_mode = &best_mode_list[i];

            kfree(mode_numbers);

            return 1;
        }
    }

    kfree(mode_numbers);
    return 0;
}



int load_hibaos(const char* filename, struct boot_info** boot_info)
{
    *boot_info = kmalloc(sizeof(struct boot_info));

    (*boot_info)->magic = BOOTINFO_HEADER_MAGIC;
    (*boot_info)->version = 1;
    (*boot_info)->si = NULL;
    (*boot_info)->mm = NULL;

    

    struct fat32* fs;
    u32* fat_entry;

    int res = fat32_main(&fs, &fat_entry, filename, (void*)HIBABL_MACHINE_KERNEL_ADDR, 512, 2048);
    fat32_free(fs, fat_entry);

    if(!res) return 0;

    void* ptr = (void*)HIBABL_MACHINE_KERNEL_ADDR;
    struct hibaboot_header* header = ptr;

    if(header->header_magic==HIBABOOT_HEADER_MAGIC)
    {
        if(header->flags&HIBABOOT_HEADER_E820)
        {
            struct e820_mm* memory_map = mmap_get();
            (*boot_info)->mm = memory_map;
        }

        if(header->flags&HIBABOOT_HEADER_SI)
        {
            struct vbe_info* vbe_header = (struct vbe_info*)HIBABL_MACHINE_SCRATCH_ADDR;
            struct vbe_mode_info* best_mode;
            int sucess = get_vbe_mode(vbe_header);
            int success = enumerate_vbe_modes(vbe_header, &best_mode);

            struct screen_info* si = kmalloc(sizeof(struct screen_info));
            (*boot_info)->si = si;

            if(sucess && success)
            {
                printk("VESA is available!\n");
                /* Framebuffer supported VESA */
        
                si->prev_video_type = BOOTINFO_SCREEN_INFO_FRAMEBUFFER;

                si->blue_pos = best_mode->LinBlueFieldPosition;
                si->blue_size = best_mode->LinBlueMaskSize;

                si->red_pos = best_mode->LinRedFieldPosition;
                si->red_size = best_mode->LinRedMaskSize;

                si->green_pos = best_mode->LinGreenFieldPosition;
                si->green_size = best_mode->LinGreenMaskSize;

                si->rsvd_pos = best_mode->LinRsvdFieldPosition;
                si->rsvd_size = best_mode->LinRsvdMaskSize;
        
                si->fb_height = best_mode->YResolution;
                si->fb_width = best_mode->XResolution;
                si->fb_depth = best_mode->BitsPerPixel;

                si->fb_base = best_mode->PhysBasePtr;
                si->fb_pitch = best_mode->LinBytesPerScanLine;

                si->fb_size =
                    best_mode->LinBytesPerScanLine *
                    best_mode->YResolution;
            }
            else
            {
                /* Framebuffer not supported, default to VGA console.*/
            }
            kfree(best_mode);
        }
        struct fat32* new_fs;
        u32* new_fat;
        fat32_main(&new_fs, &new_fat, filename, (void*)HIBABL_MACHINE_KERNEL_ADDR, 1024*1024*10, 2048);
        fat32_free(new_fs, new_fat);
        return 1;
    }
    else
    {
        printd("Did we not found the magic?: %l\n", header->header_magic);
        printd("Magic: %l\n", HIBABOOT_HEADER_MAGIC);
        return 0;
    }
}