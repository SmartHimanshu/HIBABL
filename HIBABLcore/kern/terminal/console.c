#include <HIBABL/console.h>
#include <HIBABL/string.h>

u16* vga_video_mem;
u16 terminal_x = 0;
u16 terminal_y = 0;


u8 console_make_color(u8 text_color, u8 bg_color)
{
    return (bg_color<<4) | text_color;
}

u16 console_make_char(u8 color, u8 character)
{
    return (color<<8) | character;
}

void console_put_char(u8 color, u8 character, u16 x, u16 y)
{
    vga_video_mem[x+VGA_WIDTH*y] = console_make_char(color, character);
}

u8 console_get_character(u16 x, u16 y)
{
    return vga_video_mem[x+VGA_WIDTH*y] & 0xFF;
}

void console_write_char(u8 color, u8 character)
{
    console_put_char(color, character, terminal_x, terminal_y);
    terminal_x++;
    if(terminal_x>=VGA_WIDTH)
    {
        terminal_x = 0;
        terminal_y++;
    }
}

void console_change_color(u8 color, u16 xi, u16 yi, u16 xf, u16 yf)
{
    while(yi!=yf)
    {
        for(int i = xi; i < VGA_WIDTH; i++)
        {
            console_put_char(color, console_get_character(i, yi), i, yi);
        }
        xi = 0;
        yi++;
    }
    for(int i = xi; i < xf; i++)
    {
        console_put_char(color, console_get_character(i, yi), i, yi);
    }

}

void console_print(u8 color, char* string)
{
    u16 size = strlen(string);
    for(int i = 0; i < size; i++)
    {
        console_write_char(color, string[i]);
    }
}

void console_init()
{
    vga_video_mem = (typeof(vga_video_mem))0xB8000;
    terminal_x = 0;
    terminal_y = 0;
    for(int i = 0; i < VGA_WIDTH; i++)
        for(int j = 0; j < VGA_HEIGHT; j++)
            vga_video_mem[i+VGA_WIDTH*j] = console_make_char(0, ' ');
}