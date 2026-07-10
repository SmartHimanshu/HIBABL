/* This file's main job is to load the memory map at a fixed offset so that kernel in future will
take its pointer comfortably. */

#include <HIBABL/types.h>
#include <HIBABL/mmap.h>
#include <HIBABL/memory.h>

void mmap_add_entry(struct e820_mm* map, u64 base_addr, u64 length, u32 type)
{
    map->entires[map->num_entries]->base_addr = base_addr;
    map->entires[map->num_entries]->length = length;
    map->entires[map->num_entries]->type = type;
}


void mmap_get_entry(struct e820_entries *entry)
{

}
