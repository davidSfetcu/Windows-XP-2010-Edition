// Windows XP 2010 Edition - Kernel Header
// kernel.h

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

// Kernel constants
#define KERNEL_BASE 0x00100000
#define PAGE_SIZE 4096
#define MAX_PROCESSES 256

// Typedef for common types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

// Process structure
typedef struct {
    uint32_t pid;
    uint32_t state;
    uint32_t esp;
    uint32_t eip;
    uint32_t page_directory;
    char name[32];
} process_t;

// Interrupt descriptor
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

// Interrupt descriptor table
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

// GDT Entry
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

// GDT Pointer
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

// Function declarations
void kernel_main();
void setup_gdt();
void setup_idt();
void setup_paging();
void setup_scheduler();

// VGA functions
void vga_clear();
void vga_putchar(char c);
void vga_puts(const char *str);
void vga_set_color(uint8_t fg, uint8_t bg);

// Memory functions
void* kmalloc(size_t size);
void kfree(void* ptr);
void setup_heap();

// Interrupt functions
void register_interrupt_handler(uint8_t n, void (*handler)());
void isr_handler(uint32_t num);
void irq_handler(uint32_t num);

#endif // KERNEL_H
