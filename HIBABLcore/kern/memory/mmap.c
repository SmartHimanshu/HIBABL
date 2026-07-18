/* This file's main job is to load the memory map at a fixed offset so that kernel in future will
take its pointer comfortably. */

#include <HIBABL/types.h>
#include <HIBABL/memory/mmap.h>
#include <HIBABL/machine/memory.h>
#include <HIBABL/machine/int.h>

struct e820_mm map = {0, (struct e820_entry*)HIBABL_MEMORY_MACHINE_E820_ADDR};

static void mmap_add_entry(u64 base_addr, u64 length, u32 type)
{
    map.entries[map.num_entries].base_addr = base_addr;
    map.entries[map.num_entries].length = length;
    map.entries[map.num_entries].type = type;
    map.num_entries++;
}

u64 mmap_usable_memory(void)
{
    u64 res = 0;
    for(u32 i = 0; i < map.num_entries; i++)
    {
        if(map.entries[i].type==E820_USABLE_RAM)
        {
            res+=map.entries[i].length;
        }
    }
    return res;
}
/* Returns 0 on failure, else 1 for success */
static u8 mmap_get_entry(struct e820_entry *entry, u32 *continuation)
{
    struct bios_regs regs;

    u32 addr = (u32)entry;

    regs.flags = DEFAULT_INT_FLAGS;
    regs.eax = 0xE820;
    regs.es = (u16)(addr >> 4);;
    regs.edi = addr & 0xF;
    regs.ebx = *continuation;
    regs.edx = 0x534D4150;
    regs.ecx = 20;

    bios_interrupt(0x15, &regs);

    if(regs.eax!=0x534D4150 || regs.flags&1)
    {
        return 0;
    }

    *continuation = regs.ebx;

    return 1;
}

void mmap_iterate(void)
{
    u32 continuation = 0;
    struct e820_entry entry;
    while(mmap_get_entry(&entry, &continuation))
    {
        mmap_add_entry(entry.base_addr, entry.length, entry.type);
        if(continuation==0)
        {
            break;
        }
    }
}

struct e820_mm *mmap_get(void)
{
    return &map;
}