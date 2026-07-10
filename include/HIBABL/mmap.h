#ifndef HIBABL_MEMORY_MAP_H
#define HIBABL_MEMORY_MAP_H

#include "types.h"

#define E820_USABLE_RAM         1
#define E820_RESERVED           2
#define E820_ACPI_RECLAIMABLE   3
#define E820_ACPI_NVS           4
#define E820_BAD_MEM            5

struct e820_entry
{
    u64 base_addr;
    u64 length;
    u32 type;
} HIBABL_PACKED;

struct e820_mm
{
    u32 num_entries;
    struct e820_entry* entries;
};

u64 mmap_usable_memory(void);
void mmap_iterate(void);

#endif