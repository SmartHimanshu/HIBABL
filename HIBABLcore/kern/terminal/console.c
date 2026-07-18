#include <HIBABL/terminal/console.h>
#include <HIBABL/system/string.h>

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

u8 console_get_color(u16 x, u16 y)
{
    return (vga_video_mem[x+VGA_WIDTH*y]>>8) & 0xFF;
}

void console_write_char(u8 color, u8 character)
{
    if(character=='\n')
    {
        terminal_x = 0;
        terminal_y++;
    }
    else
    {
        console_put_char(color, character, terminal_x, terminal_y);
        terminal_x++;
        if(terminal_x>=VGA_WIDTH)
        {
            terminal_x = 0;
            terminal_y++;
        }
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

void console_print_dec(u8 color, int x)
{
    char buf[12];
    int a = 0;
    if(x<0)
    {
        buf[a]='-';
        x = -x;
    }
    a++;
    if(x==0)
    {
        console_write_char(color, '0');
        return;
    }
    while(x>0)
    {
        buf[a] = '0'+x%10;
        x/=10;
        a++;
    }
    a--;
    if(buf[0]=='-')
    {
        console_write_char(color, buf[0]);
    }
    for(int i = a; i > 0; i--)
    {
        console_write_char(color, buf[i]);
    }
    return;
}

void console_print_hex(u8 color, u64 x)
{
    char buf[16];
    int a = 0;
    for(int i = 0; i < 16; i++)
    {
        u8 nice = (x>>(4*i) & 0xF);
        if(nice>=10)
        {
            buf[a] = 'a'+nice-10;
        }
        else
        {
            buf[a] = '0'+nice;
        }
        a++;
    }
    console_write_char(color, '0');
    console_write_char(color, 'x');
    for(int i = a-1; i > -1; i--)
    {
        console_write_char(color, buf[i]);
    }
}

void console_print_string(u8 color, const char* s)
{
    console_print(color, s);
}

void console_print_char(u8 color, char c)
{
    console_write_char(color, c);
}

void console_highlight_line(u8 bg_color, int y)
{
    for(int i = 0; i < VGA_WIDTH; i++)
    {
        console_put_char(console_make_color(0xF&console_get_color(i, y), bg_color), console_get_character(i, y), i, y);
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