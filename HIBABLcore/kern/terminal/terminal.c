#include <HIBABL/terminal/console.h>
#include <HIBABL/terminal/terminal.h>
#include <stdarg.h>

u8 color;

void terminal_init(void)
{
    console_init();
    color = console_make_color(VGA_WHITE, VGA_BLACK);
}

void vprintk(const char* fmt, va_list args)
{
    while(*fmt)
    {
        if(*fmt=='%')
        {
            fmt++;
            switch(*fmt)
            {
                case 'd':
                {
                    console_print_dec(color, va_arg(args, int));
                    break;
                }
                case 'x':
                {
                    console_print_hex(color, va_arg(args, u32));
                    break;
                }
                case 'c':
                {
                    console_print_char(color, va_arg(args, int));
                    break;
                }
                case 's':
                {
                    console_print_string(color, va_arg(args, char*));
                    break;
                }
            }
        }
        else
        {
            console_print_char(color, *fmt);
        }
        fmt++;
    }
}

void printk(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintk(fmt, args);
    va_end(args);
}

void panic(const char* fmt, ...)
{
    u8 tmp = color;
    color = console_make_color(VGA_LIGHT_RED, VGA_BLACK);
    va_list args;
    va_start(args, fmt);
    vprintk(fmt, args);
    va_end(args);
    color = tmp;
}

void printd(const char* fmt, ...)
{
    u8 tmp = color;
    color = console_make_color(VGA_LIGHT_GREEN, VGA_BLACK);
    va_list args;
    va_start(args, fmt);
    vprintk(fmt, args);
    va_end(args);
    color = tmp;
}
