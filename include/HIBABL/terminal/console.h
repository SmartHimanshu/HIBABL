#ifndef HIBABL_CONSOLE_H
#define HIBABL_CONSOLE_H

#include <HIBABL/types.h>

#define VGA_HEIGHT 20
#define VGA_WIDTH 80


u8 console_make_color(u8 text_color, u8 bg_color);
u16 console_make_char(u8 color, u8 character);
void console_put_char(u8 color, u8 character, u16 x, u16 y);
u8 console_get_character(u16 x, u16 y);
void console_write_char(u8 color, u8 character);
void console_change_color(u8 color, u16 xi, u16 yi, u16 xf, u16 yf);
void console_print(u8 color, char* string);
void console_print_dec(u8 color, int x);
void console_print_hex(u8 color, u64 x);
void console_print_string(u8 color, const char* s);
void console_print_char(u8 color, char c);
void console_highlight_line(u8 bg_color, int y);
void console_init();

#define VGA_BLACK          0x0
#define VGA_BLUE           0x1
#define VGA_GREEN          0x2
#define VGA_CYAN           0x3
#define VGA_RED            0x4
#define VGA_MAGENTA        0x5
#define VGA_BROWN          0x6
#define VGA_LIGHT_GREY     0x7
#define VGA_DARK_GREY      0x8
#define VGA_LIGHT_BLUE     0x9
#define VGA_LIGHT_GREEN    0xA
#define VGA_LIGHT_CYAN     0xB
#define VGA_LIGHT_RED      0xC
#define VGA_LIGHT_MAGENTA  0xD
#define VGA_YELLOW         0xE
#define VGA_WHITE          0xF

#endif