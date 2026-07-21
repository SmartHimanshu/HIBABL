#include <HIBABL/libc.h>
#include <HIBABL/types.h>

void memcpy(void *to_addr, void *from_addr, u32 bytes)
{
    u8 *new_from_addr = (u8*)from_addr;
    u8 *new_to_addr = (u8*)to_addr;
    for(u32 i = 0; i < bytes; i++)
    {
        new_to_addr[i] = new_from_addr[i];
    }
}

u64 min(u64 a, u64 b)
{
    return (a < b) ? a : b;
}