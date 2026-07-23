#include <HIBABL/boot/hiba_protocol.h>
#include <HIBABL/machine/offset.h>
#include <HIBABL/fs/fat32.h>
#include <HIBABL/machine/int.h>
#include <HIBABL/terminal/terminal.h>
#include <HIBABL/libc.h>
#include <HIBABL/machine/boot.h>
#include <HIBABL/memory/mm.h>

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

int get_vbe_mode(struct vbe_info* buffer)
{
    struct bios_regs regs = {0};
    
    regs.eax = 0x4F00;
    
    memcpy(buffer->signature, "VBE2", 4);
    regs.es = ((u32)buffer>>4)&0xFFFF;
    regs.edi = (u32)buffer&0xF;
    
    printd("buffer=%x\n", buffer);
    printd("es=%x di=%x\n", regs.es, regs.edi);

    bios_interrupt(0x10, &regs);
    
    if((regs.eax&0xFFFF) != 0x004F || memcmp(buffer->signature, "VESA", 4))
        return 0;
    
    return 1;
}

int enumerate_vbe_modes(struct vbe_info* vbe_header, struct vbe_mode_info* best_mode)
{
    /* Using disk address buffer as a scratch buffer, */

    struct vbe_mode_info* buffer = (struct vbe_mode_info*)(HIBABL_MACHINE_DISK_BUFFER_ADDR);

    u16* ptr_to_modes = (u16*)vbe_header->video_mode_ptr;

    u32 max = 0;

    for(u16 i = 0; i < vbe_header->total_memory/2; i++)
    {
        if(!get_vbe_mode_info(buffer, ptr_to_modes[i]))
        {
            return 0;
        }
        else
        {
            if(buffer->XResolution+buffer->YResolution>max)
            {
                max = buffer->XResolution+buffer->YResolution;
                memcpy(best_mode, buffer, sizeof(struct vbe_mode_info));
            }    
        }
    }
    return 1;
}



int load_hibaos(const char* filename, struct boot_info* boot_info)
{
    boot_info = kmalloc(sizeof(struct boot_info));

    boot_info->magic = BOOTINFO_HEADER_MAGIC;
    boot_info->version = 1;
    boot_info->si = NULL;
    boot_info->mm = NULL;    

    struct fat32* fs;
    u32* fat_entry;

    int res = fat32_main(fs, fat_entry, filename, (void*)0x400000, 1024, 2048);
    
    printd("Here we check for fat32 loading: %d\n", res);

    if(!res) return 0;

    void* ptr = (void*)0x400000;
    struct hibaboot_header* header = ptr;

    if(header->header_magic)
    {
        if(header->flags&HIBABOOT_HEADER_E820)
        {
            struct e820_mm* memory_map = mmap_get();
            boot_info->mm = memory_map;
        }

        if(header->flags&HIBABOOT_HEADER_SI)
        {
            struct vbe_info* vbe_header = (struct vbe_info*)HIBABL_MACHINE_SCRATCH_ADDR;
            struct vbe_mode_info* best_mode = kmalloc(sizeof(struct vbe_mode_info));
            int sucess = get_vbe_mode(vbe_header);
            int success = enumerate_vbe_modes(vbe_header, best_mode);

            struct screen_info* si = kmalloc(sizeof(struct screen_info));
        
            if(sucess && success)
            {
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
                si->fb_size = best_mode->BankSize*best_mode->YResolution;

                si->fb_pitch = best_mode->BankSize;
            }
            else
            {
                /* Framebuffer not supported, default to VGA console.*/
            }
            kfree(best_mode);
        }

        fat32_main(fs, fat_entry, filename, (void*)0x400000, 1024*1024*10, 2048);
        fat32_free(fs, fat_entry);
        return 1;
    }
    else
    {
        printd("Did we not found the magic?: %x\n", header->header_magic);
        return 0;
    }
}