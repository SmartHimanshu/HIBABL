/* This file is responsible for making a custom memory allocator */

/*
It's implementation is a simple linked list.
We map 4MiB bytes initially and then we continue to 
give malloc next 1MiB if we run out of space.
*/

#include <HIBABL/types.h>
#include <HIBABL/memory.h>
#include <HIBABL/mmap.h>
#include <HIBABL/mm.h>

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
            u64 offset = (0x100000+HIBABL_MEMORY_BITMAP_SIZE)/4096;
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
                u64 nxt_offset = ((map->entries[i].base_addr+map->entries[i].length)/4096);
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
                for(u64 j = map->entries[i].base_addr/4096; j < offset_a/4096; j++)
                {
                    page_info_bitmap[j/8] &= ~(1<<(j%8));
                }
            }
        }
    }
}

void* pmm_alloc_page(void)
{
    for(u64 i = 0; i < HIBABL_MEMORY_BITMAP_SIZE; i++)
    {
        if(page_info_bitmap[i]!=0xFF)
        {
            for(u64 bit = 0; bit < 8; bit++)
            {
                if((~page_info_bitmap[i]) & (1<<bit))
                {
                    page_info_bitmap[i] |= (1<<bit);
                    return (void*) (((i*8)+bit)*4096);
                }
            }
        }
    }
    return (void*) 0x00;
}

static void mark_pages_used(size_t start, size_t end)
{
    for(int bit = 0; bit < 8-(start%8); bit++)
    {
        page_info_bitmap[start/8] |= (128>>bit);
    }
    for(int bit = 0; bit < end%8; bit++)
    {
        page_info_bitmap[end/8] |= (1<<bit);
    }
    size_t sizee = ((end-(end%8)) - (start+(start%8)))/8;
    for(int i = 0; i < sizee; i++)
    {
        page_info_bitmap[((start+(start%8))/8)+i] = 0xFF;
    }
}

void* pmm_alloc_pages(size_t count)
{
    u64 start;
    u64 length = 0;
    for(u64 bit; bit < HIBABL_MEMORY_BITMAP_SIZE*8; bit++)
    {
        u8 thing = page_info_bitmap[bit/8] >> bit & 1;
        if(!thing)
        {
            if(length==0)
            {
                start = bit;
            }
            length++;
            if(count==length)
            {
                return (void*)(start*4096);
            }
        }
    }
    return (void*) 0x00;
}

void pmm_free_page(void* ptr)
{
    u64 memory_addr = (u64)ptr;
    u64 ind = memory_addr/4096;
    page_info_bitmap[ind/8] &= ~(1<<(ind%8));   
}



void* heap_malloc(size_t size)
{

}