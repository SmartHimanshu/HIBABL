This is my own bootloader, written entirely in GAS syntax assembly and C.
Its primary goal for future versions is to basically decompress itself 
from gzip format (In future maybe xz compression) and to put the main
core code at an address and then load my next project's Operating System
from FAT32 parition (In future my own custom FS) to a nice comfortable
memory address. Later adding support for multiboot headers, etc.