#include <HIBABL/system/string.h>
#include <HIBABL/types.h>
#include <HIBABL/memory/mm.h>


u16 strlen(char* string)
{
    char* base = string;
    while(*string!=0)
    {
        string++;
    }
    return string-base;
}

int strncmp(char* a, char* b, u32 size)
{
    for(int i = 0 ; i < size; i++)
    {
        if(a[i]!=b[i])
        {
            return (a[i] > b[i]) ? 1 : -1;
        }
    }
    return 0;
}

int strcmp(char* a, char* b)
{
    while(*a!='\0' && *b!='\0')
    {
        if(*a!=*b)
        {
            return (*a > *b) ? 1 : -1;
        }
        a++;
        b++;
    }
    if(*a!=*b)
    {
        return (*a > *b) ? 1 : -1;
    }
    return 0;
}

void splitsn(char** res, const char* string, char split_char, u32 size_split, u32* count)
{
    u32 current_index = 0;
    u32 size = 8;
    *res = kmalloc((size*size_split+1)*sizeof(char));
    for(u32 i = 0; i < size*size_split; i++)
    {
        (*res)[i] = ' ';
    }
    char* last_seen = string;
    u32 countt = 0;
    while(*string!='\0')
    {
        if(current_index==size)
        {
            size*=2;
            *res = krealloc(*res, (size*size_split+1)*sizeof(char));
            for(u32 i = current_index; i < size*size_split; i++)
            {
                (*res)[i] = ' ';
            }
        }
        if(*string == split_char)
        {
            last_seen = string+1;
            countt++;
            current_index++;
        }
        else
        {
            (*res)[countt*size_split + (u32)(string-last_seen)] = *string;
        }
        string++;
    }
    *count = countt+1;
    (*res)[size*size_split] = '\0';
}