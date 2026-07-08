ENTRY(start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0x8200;

    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(.bss*)
    }
}