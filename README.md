# Windows XP 2010 Edition - Minimalist OS

A educational, minimalist operating system inspired by Windows XP architecture, built from scratch in C, C++, and x86 Assembly.

## Overview

**Windows XP 2010 Edition** is an open-source, educational OS kernel designed to demonstrate fundamental operating system concepts including:
- Bootloader and kernel initialization
- Memory management (paging, segmentation, virtual memory)
- Process scheduling and task management
- Interrupt and exception handling
- File system implementation (FAT12)
- Device drivers (keyboard, disk, VGA)
- Shell and command-line interface

## System Architecture

### Bootloader (x86-16)
- **Location**: `boot/bootloader.asm`
- **Size**: 512 bytes (single sector)
- **Functionality**:
  - Real mode initialization
  - Disk I/O (load kernel from disk)
  - A20 line enabling
  - Protected mode setup
  - GDT (Global Descriptor Table) loading

### Kernel (C/C++)
- **Entry Point**: `kernel/kernel.c`
- **Main Components**:
  - Kernel initialization
  - Memory manager (paging, heap allocation)
  - Interrupt descriptor table (IDT)
  - Task scheduler (round-robin)
  - System calls interface

### Memory Management
- **Virtual Memory**: 4MB kernel space, 4MB user space
- **Paging**: 4KB pages with page directory and page tables
- **Segmentation**: Code, data, and stack segments
- **Heap**: Simple buddy allocator

### Interrupt Handling
- **IDT Setup**: 256 interrupt handlers
- **CPU Exceptions**: Division by zero, page fault, etc.
- **IRQ Handlers**: Timer (IRQ0), Keyboard (IRQ1)
- **System Calls**: User-mode to kernel-mode transition

### File System (FAT12)
- **Boot Sector**: Volume identification
- **FAT Table**: File allocation table
- **Root Directory**: File metadata
- **Data Clusters**: Actual file content
- **Operations**: Read, write, list, delete

### Device Drivers

#### VGA Driver
- **Mode**: 80x25 text mode
- **Colors**: 16 foreground + 16 background
- **Functions**: Write character, clear screen, cursor control

#### Keyboard Driver
- **Interrupt-driven**: IRQ1 handler
- **Key mapping**: ASCII conversion
- **Buffer**: Input queue for shell

#### Disk Driver
- **Hardware**: Floppy disk controller (legacy)
- **Operations**: Read sectors, write sectors
- **I/O**: Programmed I/O (PIO)

### Shell
- **Command Interface**: Command-line interpreter
- **Built-in Commands**:
  - `dir` - List files
  - `cat` - Read file
  - `write` - Write file
  - `del` - Delete file
  - `cls` - Clear screen
  - `help` - Display help
  - `reboot` - System reboot

## Project Structure

```
Windows XP 2010 Edition/
├── boot/
│   ├── bootloader.asm       # x86 bootloader
│   └── gdt.asm              # GDT definition
├── kernel/
│   ├── kernel.c             # Kernel main
│   ├── kernel.h
│   ├── memory.c             # Memory management
│   ├── interrupt.c          # IDT and IRQ handling
│   ├── scheduler.c          # Task scheduler
│   └── syscall.c            # System calls
├── drivers/
│   ├── vga.c                # VGA display driver
│   ├── keyboard.c           # Keyboard driver
│   ├── disk.c               # Disk I/O driver
│   └── timer.c              # PIT timer driver
├── fs/
│   ├── fat12.c              # FAT12 implementation
│   └── inode.c              # File metadata
├── shell/
│   ├── shell.c              # Command interpreter
│   └── commands.c           # Built-in commands
├── utils/
│   ├── string.c             # String utilities
│   └── math.c               # Math utilities
├── Makefile                 # Build configuration
└── README.md                # This file
```

## Building

### Prerequisites
- GCC cross-compiler (i686-elf-gcc)
- NASM assembler
- GNU Make
- QEMU (for emulation)
- xorriso or mkisofs (for ISO creation)

### Compilation

```bash
# Full build
make all

# Bootloader only
make bootloader

# Kernel only
make kernel

# Drivers
make drivers

# Clean build artifacts
make clean

# Create ISO image
make iso
```

### Running

```bash
# Run in QEMU
qemu-system-i386 -cdrom windows_xp_2010.iso

# Run with debugging
qemu-system-i386 -cdrom windows_xp_2010.iso -s -S
```

## Technical Details

### Boot Process
1. BIOS loads bootloader from sector 0
2. Bootloader enables A20 line
3. Bootloader loads kernel from disk
4. Bootloader switches to Protected Mode
5. Bootloader jumps to kernel entry point
6. Kernel initializes IDT, GDT, paging
7. Kernel starts scheduler
8. First shell process launches

### Memory Layout
```
0x00000000 - 0x00000FFF: Interrupt Vector Table (IVT)
0x00001000 - 0x0009FBFF: Kernel code and data
0x0009FC00 - 0x0009FFFF: EBDA (Extended BIOS Data Area)
0x000A0000 - 0x000BFFFF: Video memory
0x000C0000 - 0x000FFFFF: ROM BIOS
0x00100000+: Extended memory (kernel paging)
```

### System Calls
```
int 0x80  - System call interrupt
EAX=0     - sys_exit(code)
EAX=1     - sys_write(fd, buffer, size)
EAX=2     - sys_read(fd, buffer, size)
EAX=3     - sys_open(filename, flags)
EAX=4     - sys_close(fd)
EAX=5     - sys_create(filename)
```

### Task States
- **READY**: Waiting to run
- **RUNNING**: Currently executing
- **BLOCKED**: Waiting for I/O or event
- **TERMINATED**: Finished execution

## Features

✅ Real x86 bootloader  
✅ Protected mode kernel  
✅ Virtual memory (paging)  
✅ Process scheduling (round-robin)  
✅ Interrupt handling (256 handlers)  
✅ FAT12 file system  
✅ Device drivers (VGA, keyboard, disk)  
✅ Shell with built-in commands  
✅ System call interface  
✅ Error handling and exceptions  

## Educational Purpose

This project is designed for learning:
- Operating system internals
- Bootloader development
- x86 assembly language
- Memory management concepts
- Interrupt and exception handling
- File system design
- Device driver development
- Scheduling algorithms

## Limitations

- **Single CPU**: No multicore support
- **32-bit**: x86 architecture only
- **Legacy Hardware**: Floppy disk, VGA
- **No Networking**: Network stack not implemented
- **No GUI**: Command-line only interface
- **Limited Security**: No user privilege levels
- **Educational Only**: Not production-ready

## References

- Intel 64 and IA-32 Architectures Software Developer's Manual
- OSDEV.org - Operating System Development
- FAT12 File System Specification
- x86 Assembly Language Reference

## License

Open source educational project. Use for learning purposes.

## Author

Created as an educational OS kernel demonstration.

---

**Note**: This is a simplified, educational OS. Real Windows XP is a complex proprietary system. This project demonstrates fundamental OS concepts using clean, understandable code.
