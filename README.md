# HIBABL — HIBA Mark-1 Bootloader

Welcome to **HIBABL (HIBA Mark-1 Bootloader)!**

HIBABL is the first bootloader developed as part of the HIBA operating system project. It is a custom, multi-stage x86_64 bootloader designed for legacy BIOS systems.

## What is HIBABL?

HIBABL is a three-stage bootloader that initializes the system environment, provides memory management facilities, interacts with legacy BIOS services, and prepares the environment for loading and executing the kernel.

## Features

### Boot and disk support
1. x86_64 architecture support
2. Legacy BIOS boot support
3. Three-stage boot architecture
4. CHS and LBA disk addressing support
5. BIOS disk I/O
6. Workarounds for buggy BIOS implementations
7. FAT32 filesystem support

### BIOS and hardware support
8. Custom BIOS thunk for switching between protected mode and real mode
9. BIOS VBE initialization and mode enumeration
10. Framebuffer information handoff to the kernel
11. BIOS E820 memory map support
12. VGA text mode support

### Memory and processor initialization
13. Physical Memory Manager (PMM)
14. Virtual Memory Manager (VMM)
15. Global Descriptor Table (GDT) setup

### Other features
16. Basic `printf` implementation
17. Custom HIBA boot protocol for communication between the bootloader and kernel

## Current Status

HIBABL currently searches for a kernel image named `KERNEL.BIN` on its FAT32 filesystem.

The build system also expects a `KERNEL.BIN` or sample kernel image to be present during the build process.

## Project

HIBABL is part of the broader HIBABLOS operating system development effort.
