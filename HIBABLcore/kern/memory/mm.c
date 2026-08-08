/*
 * HIBABL
 * Copyright (C) 2026 Himanshu Bakshi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* This file is responsible for making a custom memory allocator */

/*
It's implementation is a doubly linked list.
We map 4MiB bytes initially and then we continue to
give malloc next 1MiB if we run out of space.
*/

#include <HIBABL/types.h>
#include <HIBABL/machine/memory.h>
#include <HIBABL/memory/mmap.h>
#include <HIBABL/memory/mm.h>
#include <HIBABL/system/magic.h>
#include <HIBABL/system/error.h>
#include <HIBABL/terminal/terminal.h>
#include <HIBABL/libc.h>

u8 page_info_bitmap[HIBABL_MEMORY_BITMAP_SIZE] = {0};
struct heap_header header;
// If page_info_bitmap == 1 means used.


static void mark_pages_used(size_t start, size_t size)
{
    start /= HIBABL_MEMORY_PAGE_SIZE;
    size = (size + HIBABL_MEMORY_PAGE_SIZE - 1)/HIBABL_MEMORY_PAGE_SIZE;

    for (size_t i = start; i < start + size; i++) {
        page_info_bitmap[i/8] |= 1 << (i%8);
    }
}

void pmm_free_page(void* ptr)
{
    u32 memory_addr = (u32)ptr;
    u32 ind = memory_addr/HIBABL_MEMORY_PAGE_SIZE;
    page_info_bitmap[ind/8] &= ~(1<<(ind%8));
}

void pmm_init(void)
{
    struct e820_mm *map = mmap_get();
    mark_pages_used(0, 0x100000);

    for(u64 i = 0; i < map->num_entries; i++)
    {
        if(map->entries[i].type != E820_USABLE_RAM)
        {
            mark_pages_used(
                map->entries[i].base_addr,
                map->entries[i].length);
        }
    }
}

void* pmm_alloc_page(void)
{
    for(u32 i = 0; i < HIBABL_MEMORY_BITMAP_SIZE * 8; i++)
    {
        if(page_info_bitmap[i/8] == 0xFF)
            continue;
        if(!(page_info_bitmap[i/8] & (1<<(i%8))))
        {
            page_info_bitmap[i/8] |= 1<<(i%8);
            return (void*) (i*HIBABL_MEMORY_PAGE_SIZE);
        }
    }
    return (void*) 0x00;
}

void* pmm_alloc_pages(size_t count)
{
    size_t start;
    size_t length = 0;
    for(size_t i = 0; i < HIBABL_MEMORY_BITMAP_SIZE*8; i++)
    {
        if(!(page_info_bitmap[i/8] & (1 << (i%8))))
        {
            if(length==0)
            {
                start = i;
            }
            length++;
            if(count==length)
            {
                mark_pages_used(start * HIBABL_MEMORY_PAGE_SIZE,
                    length * HIBABL_MEMORY_PAGE_SIZE);
                return (void*)(start*HIBABL_MEMORY_PAGE_SIZE);
            }
        }
        else
        {
            length = 0;
        }
    }
    return (void*) 0x00;
}

void heap_init(void)
{
    u32 aligned_struct_size = ALIGN_UP(sizeof(struct heap_block), HEAP_ALIGNMENT);

    header.header_magic = HEAP_HEADER_MAGIC;
    header.free_blocks = 1;
    header.total_blocks = 1;

    u64 total_needed_pages = (HIBABL_MEMORY_HEAP_INITIAL_SIZE+aligned_struct_size+HIBABL_MEMORY_PAGE_SIZE-1)/HIBABL_MEMORY_PAGE_SIZE;

    header.total_pages = total_needed_pages;

    void* addr_of_first_heap = pmm_alloc_pages(total_needed_pages);

    header.head = (struct heap_block*)addr_of_first_heap;
    header.tail = header.head;

    header.head->block_magic = HEAP_BLOCK_MAGIC;
    header.head->free = 1;
    header.head->next = NULL;
    header.head->prev = NULL;
    header.head->size = total_needed_pages*HIBABL_MEMORY_PAGE_SIZE - aligned_struct_size;
}

void* heap_malloc(size_t size)
{
    size = ALIGN_UP(size, HEAP_ALIGNMENT);
    u32 aligned_struct_size = ALIGN_UP(sizeof(struct heap_block), HEAP_ALIGNMENT);
    struct heap_block* runner = header.head;
    while(runner!=NULL)
    {
        if(runner->free && runner->size>=size)
        {
            if(runner->size>=size+aligned_struct_size+HEAP_ALIGNMENT)
            {
                struct heap_block* k = (struct heap_block*)((u32)runner+aligned_struct_size+size);
                k->size = runner->size - size - aligned_struct_size;
                runner->size = size;
                k->block_magic = HEAP_BLOCK_MAGIC;
                k->free = 1;
                k->next = runner->next;
                k->prev = runner;
                if(runner->next==NULL)
                {
                    header.tail = k;
                }
                else
                {
                    runner->next->prev = k;
                }
                runner->next = k;
                runner->free = 0;
                return (void*)((u32)runner + aligned_struct_size);
            }
            else if(runner->size>=size)
            {
                runner->free = 0;
                return (void*)((u32)runner + aligned_struct_size);
            }
        }
        runner = runner->next;
    }

    //This is the case where the runner has exhausted itself and we need to allocate new memory

    u32 total_needed_size = ((aligned_struct_size+size+HIBABL_MEMORY_PAGE_SIZE-1)/HIBABL_MEMORY_PAGE_SIZE)*HIBABL_MEMORY_PAGE_SIZE;
    u32 needed_pages = total_needed_size/HIBABL_MEMORY_PAGE_SIZE;

    void* end = pmm_alloc_pages(needed_pages);

    struct heap_block* newtail = (struct heap_block*)end;
    newtail->size = total_needed_size-aligned_struct_size;
    newtail->block_magic = HEAP_BLOCK_MAGIC;
    newtail->free = 0;
    newtail->next = NULL;
    newtail->prev = header.tail;
    header.tail->next = newtail;
    header.tail = newtail;

    if(total_needed_size>=size+HEAP_ALIGNMENT+(aligned_struct_size*2))
    {
        struct heap_block* newnewtail = (struct heap_block*)((u32)end+(aligned_struct_size*2)+size);
        newnewtail->block_magic = HEAP_BLOCK_MAGIC;
        newnewtail->free = 1;
        newnewtail->next = NULL;
        newnewtail->prev = newtail;
        newnewtail->size = total_needed_size - size - HEAP_ALIGNMENT -(aligned_struct_size*2);
        newtail->size = size;
        newtail->next = newnewtail;
        header.tail = newnewtail;
    }

    return (void*)((u32)end+aligned_struct_size);
}

int heap_free(void* addr)
{
    u32 aligned_struct_size = ALIGN_UP(sizeof(struct heap_block), HEAP_ALIGNMENT);
    addr = (void*)((u32)addr-aligned_struct_size);
    if((u32)addr%HEAP_ALIGNMENT)
    {
        return ERRINV;
    }
    struct heap_block* block = (struct heap_block*)addr;
    if(block->block_magic != HEAP_BLOCK_MAGIC)
    {
        panic("Valid heap not passed\n");
        return ERRINV;
    }
    block->free=1;
    if(block->next!=NULL)
    {
        if(block->next->free==1 && (u64)block+block->size+aligned_struct_size == (u64)block->next)
        {
            if(block->next==header.tail)
            {
                header.tail = block;
            }
            block->size += aligned_struct_size + block->next->size;
            if(block->next->next!=NULL)
            {
                block->next->next->prev = block;
            }
            block->next = block->next->next;
        }
    }
    if(block->prev!=NULL)
    {
        if(block->prev->free==1 && (u64)block->prev+block->prev->size+aligned_struct_size == (u64)block)
        {
            if(block==header.tail)
            {
                header.tail = block->prev;
            }
            block->prev->size += aligned_struct_size + block->size;
            if(block->next!=NULL)
            {
                block->next->prev = block->prev;
            }
            block->prev->next = block->next;
        }
    }
    return SUCCESS;
}

int heap_magic_check(void)
{
    struct heap_block* runner = header.head;

    if(header.header_magic!=HEAP_HEADER_MAGIC)
            return -1;

    while(runner!=NULL)
    {
        if(runner->block_magic!=HEAP_BLOCK_MAGIC)
            return -1;
        runner = runner->next;
    }
    return SUCCESS;
}

void* heap_realloc(void* ptr, size_t size)
{
    if(ptr==NULL)
    {
        return heap_malloc(size);
    }

    if(size==0)
    {
        heap_free(ptr);
        return NULL;
    }

    struct heap_block* block = ptr - sizeof(struct heap_block);
    u32 old_size = block->size;

    void* res = heap_malloc(size);

    if (res == NULL)
        return NULL;

    memcpy(res, ptr, min(size, old_size));

    heap_free(ptr);

    return res;
}

void allocator_init(void)
{
    mmap_iterate();
    pmm_init();
    heap_init();
}

/* These implementations of the allocator is to strictly be used for debugging purposes. */

void* dmalloc(size_t size)
{
    printd("Allocated memory of size: %x\n", size);
    return heap_malloc(size);
}

int dfree(void* addr)
{
    printd("Freed memory at address: %x\n", (u64)addr);
    return heap_free(addr);
}

void* drealloc(void* ptr, size_t size)
{
    void* new_ptr = heap_realloc(ptr, size);
    printd("Reallocated memory with:\nOf size : %d\nFrom address : %d\nTo address : %d\n", size, (u64)ptr, (u64)new_ptr);
    return new_ptr;
}

/* These are the master malloc functions used by the core. */

void* kmalloc(size_t size)
{
    if (size == 0) {
        printd("%x\n", __builtin_return_address(2));
    }
    return dmalloc(size);
}

int kfree(void* addr)
{
    return dfree(addr);
}

void* krealloc(void* ptr, size_t size)
{
    return heap_realloc(ptr, size);
}
