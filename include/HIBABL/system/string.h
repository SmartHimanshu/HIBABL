#ifndef HIBABL_STRING_H
#define HIBABL_STRING_H

#include <HIBABL/types.h>

u16 strlen(char* string);
int strncmp(char* a, char* b, u32 size);
int strcmp(char* a, char* b);

#endif