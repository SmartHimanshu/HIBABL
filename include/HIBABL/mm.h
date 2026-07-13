#ifndef HIBABL_MEMORY_ALLOCATOR_H
#define HIBABL_MEMORY_ALLOCATOR_H

#include "types.h"

#define HEAP_ALIGNMENT 8

#define ALIGN_UP(x,a) \
    (((x)+(a)-1) & ~((a)-1))

struct heap_block
{
    u64 block_magic;
    u64 size;
    u8 free;
    struct heap_block* prev;
    struct heap_block* next;
} HIBABL_PACKED;


struct heap_header
{
    u64 header_magic;
    u64 total_blocks;
    u64 total_pages;
    u64 free_blocks;
    struct heap_block* head;
    struct heap_block* tail;
};


void pmm_init(void);
void* pmm_alloc_page(void);
void* pmm_alloc_pages(size_t count);
void* heap_malloc(size_t size);

#endif