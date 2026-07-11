/* This file is responsible for making a custom memory allocator */

/*
It's implementation is a simple linked list.
We map 4KiB bytes initially and then we continue to 
give malloc next 4KiB if we run out of space.
*/

#include <HIBABL/types.h>
#include <HIBABL/memory.h>
#include <HIBABL/mmap.h>
#include <HIBABL/mm.h>

u8* page_info_bitmap;
struct heap_block* head = NULL;
struct heap_block* tail = NULL;

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
    size_t run_start = 0;
    size_t run_len = 0;

    for(size_t page = 0; page < HIBABL_MEMORY_BITMAP_SIZE * 8; page++)
    {
        u8 byte = page_info_bitmap[page / 8];
        u8 bit  = (byte >> (page % 8)) & 1;

        if(bit == 0)  // free page
        {
            if(run_len == 0)
                run_start = page;

            run_len++;

            if(run_len == count)
            {
                for(size_t p = run_start;
                    p < run_start + count;
                    p++)
                {
                    page_info_bitmap[p / 8] |=
                        (1 << (p % 8));
                }

                return (void*)(run_start * 4096);
            }
        }
        else
        {
            run_len = 0;
        }
    }

    return NULL;
}

void pmm_free_page(void* ptr)
{
    u64 memory_addr = (u64)ptr;
    u64 ind = memory_addr/4096;
    page_info_bitmap[ind/8] &= ~(1<<(ind%8));   
}



void* heap_malloc(size_t size)
{
    size = (size + 7) & ~7;

    if(head == NULL)
    {
        size_t total_size = ((size + sizeof(struct heap_block) + 4095) / 4096) * 4096;

        head = (struct heap_block*)pmm_alloc_pages(total_size / 4096);


        head->free = 0;
        head->size = size;

        size_t remaining = total_size - sizeof(struct heap_block) - size;

        if(remaining > sizeof(struct heap_block))
        {
            tail = (struct heap_block*)((u64)head + sizeof(struct heap_block) + size);

            tail->free = 1;
            tail->size =
                remaining - sizeof(struct heap_block);
            tail->next = NULL;

            head->next = tail;
        }
        else
        {
            tail = head;
            head->next = NULL;
        }

        return (void*)
        (
            (u64)head +
            sizeof(struct heap_block)
        );
    }

    struct heap_block* runner = head;

    while(runner != NULL)
    {
        if(runner->free)
        {
            if(runner->size >=
               size + sizeof(struct heap_block) + 8)
            {
                struct heap_block* nextt =
                    runner->next;

                struct heap_block* new =
                    (struct heap_block*)
                    (
                        (u64)runner +
                        sizeof(struct heap_block) +
                        size
                    );

                u64 old_size = runner->size;

                runner->size = size;
                runner->free = 0;
                runner->next = new;

                new->size =
                    old_size -
                    size -
                    sizeof(struct heap_block);

                new->free = 1;
                new->next = nextt;

                if(runner == tail)
                    tail = new;

                return (void*)
                (
                    (u64)runner +
                    sizeof(struct heap_block)
                );
            }
            else if(runner->size >= size)
            {
                runner->free = 0;

                return (void*)
                (
                    (u64)runner +
                    sizeof(struct heap_block)
                );
            }
        }

        runner = runner->next;
    }

    /* Since we didn't find enough memory, here we are supposed to get more memory via pages. */

    u64 num_pages = (size/4096)+1;
    
    if(size%4096)
    {

    }

    return NULL;
}