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