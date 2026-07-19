#include <HIBABL/system/string.h>


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