#ifndef HIBABL_TERMINAL_H
#define HIBABL_TERMINAL_H


void terminal_init(void);
void printk(const char* fmt, ...);
void panic(const char* fmt, ...);
void printd(const char* fmt, ...);


#endif