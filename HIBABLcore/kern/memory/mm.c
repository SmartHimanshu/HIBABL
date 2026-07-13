/* This file is responsible for making a custom memory allocator */

/*
It's implementation is a doubly linked list.
We map 4MiB bytes initially and then we continue to 
give malloc next 1MiB if we run out of space.
*/

#include <HIBABL/types.h>
#include <HIBABL/memory.h>
#include <HIBABL/mmap.h>
#include <HIBABL/mm.h>
#include <HIBABL/magic.h>

u8* page_info_bitmap;
struct heap_header header;
// If page_info_bitmap == 1 means used. 

void pmm_init(void)
{
    struct e820_mm *map = mmap_get();
    for(u64 i = 0; i < map->num_entries; i++)
    {
        if(map->entries[i].base_addr == 0x100000)
        {
            page_info_bitmap = (u8*)map->entries[i].base_addr;
            for(u64 j = 0; j < HIBABL_MEMORY_BITMAP_SIZE; j++)
            {
                page_info_bitmap[j] = 0xFF;
            }
            u64 offset = (0x100000+HIBABL_MEMORY_BITMAP_SIZE)/HIBABL_MEMORY_PAGE_SIZE;
            if(map->entries[i].base_addr+map->entries[i].length>HIBABL_MEMORY_DYNAMIC_COVERAGE)
            {
                for(u64 j = offset; j < offset+(HIBABL_MEMORY_BITMAP_SIZE*8); j++)
                {
                    page_info_bitmap[j/8] &= ~(1<<(j%8));
                }
                break;
            }
            else
            {
                u64 nxt_offset = ((map->entries[i].base_addr+map->entries[i].length)/HIBABL_MEMORY_PAGE_SIZE);
                for(u64 j = offset; j < offset+nxt_offset; j++)
                {
                    page_info_bitmap[j/8] &= ~(1<<(j%8));
                }
                continue;
            }
        }
        if(map->entries[i].type == E820_USABLE_RAM)
        {
            u64 offset_a = map->entries[i].base_addr+map->entries[i].length;
            if(offset_a>HIBABL_MEMORY_DYNAMIC_COVERAGE)
            {
                for(u64 j = map->entries[i].base_addr/4096; j < (HIBABL_MEMORY_BITMAP_SIZE*8); j++)
                {
                    page_info_bitmap[j/8] &= ~(1<<(j%8));
                }
                break;
            }
            else
            {
                for(u64 j = map->entries[i].base_addr/HIBABL_MEMORY_PAGE_SIZE; j < offset_a/HIBABL_MEMORY_PAGE_SIZE; j++)
                {
                    page_info_bitmap[j/8] &= ~(1<<(j%8));
                }
            }
        }
    }
}

void* pmm_alloc_page(void)
{
    for(u32 i = 0; i < HIBABL_MEMORY_BITMAP_SIZE; i++)
    {
        if(page_info_bitmap[i]!=0xFF)
        {
            for(u32 bit = 0; bit < 8; bit++)
            {
                if((~page_info_bitmap[i]) & (1<<bit))
                {
                    page_info_bitmap[i] |= (1<<bit);
                    return (void*) (((i*8)+bit)*HIBABL_MEMORY_PAGE_SIZE);
                }
            }
        }
    }
    return (void*) 0x00;
}

static void mark_pages_used(size_t start, size_t end)
{
    for(u8 bit = 0; bit < 8-(start%8); bit++)
    {
        page_info_bitmap[start/8] |= (128>>bit);
    }
    for(u8 bit = 0; bit < end%8; bit++)
    {
        page_info_bitmap[end/8] |= (1<<bit);
    }
    size_t sizee = ((end-(end%8)) - (start+(start%8)))/8;
    for(u8 i = 0; i < sizee; i++)
    {
        page_info_bitmap[((start+(start%8))/8)+i] = 0xFF;
    }
}

void* pmm_alloc_pages(size_t count)
{
    u64 start;
    u64 length = 0;
    for(u64 bit = 0; bit < HIBABL_MEMORY_BITMAP_SIZE*8; bit++)
    {
        u8 thing = page_info_bitmap[bit/8] >> (bit%8) & 1;
        if(!thing)
        {
            if(length==0)
            {
                start = bit;
            }
            length++;
            if(count==length)
            {
                mark_pages_used(start, start+length);
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

void pmm_free_page(void* ptr)
{
    u32 memory_addr = (u32)ptr;
    u32 ind = memory_addr/HIBABL_MEMORY_PAGE_SIZE;
    page_info_bitmap[ind/8] &= ~(1<<(ind%8));   
}

void heap_init(void)
{
    header.header_magic = HEAP_HEADER_MAGIC;
    header.free_blocks = 1;
    header.total_blocks = 1;
    u64 total_needed_pages = (HIBABL_MEMORY_HEAP_INITIAL_SIZE+sizeof(struct heap_block)+HIBABL_MEMORY_PAGE_SIZE-1)/HIBABL_MEMORY_PAGE_SIZE;
    header.total_pages = total_needed_pages;
    void* addr_of_first_heap = pmm_alloc_pages(total_needed_pages);
    header.head = (struct heap_block*)addr_of_first_heap;
    header.tail = header.head;
    header.head->block_magic = HEAP_BLOCK_MAGIC;
    header.head->free = 1;
    header.head->next = NULL;
    header.head->prev = NULL;
    header.head->size = total_needed_pages*HIBABL_MEMORY_PAGE_SIZE - sizeof(struct heap_block);
}

void* heap_malloc(size_t size)
{
    struct heap_block* runner = header.head;
    while(runner!=NULL)
    {
        if(runner->free && runner->size>=size)
        {
            if(runner->size==size)
            {
                runner->free = 0;
                return (void*)((u32)runner + sizeof(struct heap_block));
            }
            else
            {
                
            }
        }
        runner = runner->next;
    }
    return (void*)0x00;
}