#include <HIBABL/terminal/terminal.h>
#include <HIBABL/memory/mm.h>
#include <HIBABL/disk/disk.h>
#include <HIBABL/fs/fat32.h>
#include <HIBABL/fs/vfs.h>

void __attribute__ ((noreturn))
kmain()
{
    terminal_init();
    printk("Welcome to HIBA bootloader!\n");
    allocator_init();
    disk_geometry_init();
    printk("Have we succeded? : %d\n", fat32_main("CORE    BIN", (void*)0x400000, 1024, 2048));
    struct file_path* path;
    fat32_parse_path("coot/bootmgr.img", path);
    while(1) { };
}