/* This file's main job is to load the memory map at a fixed offset so that kernel in future will
take its pointer comfortably. */

#include <HIBABL/types.h>
#include <HIBABL/mmap.h>
#include <HIBABL/memory.h>
#include <HIBABL/int.h>

struct e820_mm* map = {0, (struct e820_entry*)HIBABL_MEMORY_MACHINE_E820_ADDR};

void mmap_add_entry(u64 base_addr, u64 length, u32 type)
{
    map->entries[map->num_entries].base_addr = base_addr;
    map->entries[map->num_entries].length = length;
    map->entries[map->num_entries].type = type;
    map->num_entries++;
}

u64 mmap_usable_memory(void)
{
    u64 res = 0;
    for(int i = 0; i < map->num_entries; i++)
    {
        if(map->entries[i].type==E820_USABLE_RAM)
        {
            res+=map->entries[i].length;
        }
    }
    return res;
}

void mmap_get_entry(struct e820_entries *entry)
{
    struct bios_regs regs;
    regs.eax = 0xE820;
    
}
