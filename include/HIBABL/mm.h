#ifndef HIBABL_MEMORY_ALLOCATOR_H
#define HIBABL_MEMORY_ALLOCATOR_H

#include "types.h"

struct heap_block
{
    u64 size;
    u8 free;
    struct heap_block* next;
} HIBABL_PACKED;

void pmm_init(void);
void* pmm_alloc_page(void);
void* pmm_alloc_pages(size_t count);
void* heap_malloc(size_t size);

#endif