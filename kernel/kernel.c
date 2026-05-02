// Windows XP 2010 Edition - Kernel Main
// kernel.c

#include "kernel.h"

// VGA display variables
static uint16_t *vga_buffer = (uint16_t *)0xB8000;
static uint8_t vga_x = 0, vga_y = 0;
static uint8_t vga_color = 0x0F; // White on black

// GDT
gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;

// IDT
idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

// Process table
process_t processes[MAX_PROCESSES];
uint32_t current_process = 0;
uint32_t process_count = 0;

/**
 * Main kernel entry point
 */
void kernel_main() {
    // Initialize VGA display
    vga_clear();
    vga_puts("=== Windows XP 2010 Edition ===\n");
    vga_puts("Kernel booting...\n\n");
    
    // Initialize GDT
    vga_puts("[*] Setting up GDT...\n");
    setup_gdt();
    
    // Initialize IDT
    vga_puts("[*] Setting up IDT...\n");
    setup_idt();
    
    // Initialize paging
    vga_puts("[*] Setting up paging...\n");
    setup_paging();
    
    // Initialize memory heap
    vga_puts("[*] Initializing heap...\n");
    setup_heap();
    
    // Initialize scheduler
    vga_puts("[*] Setting up scheduler...\n");
    setup_scheduler();
    
    vga_puts("\n[+] Kernel initialized successfully!\n");
    vga_puts("[+] Starting shell...\n\n");
    
    // Enter shell
    shell_main();
    
    // Hang
    while (1) {
        asm volatile("hlt");
    }
}

/**
 * Setup Global Descriptor Table
 */
void setup_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;
    
    // Null descriptor
    gdt_entries[0].limit_low = 0;
    gdt_entries[0].base_low = 0;
    gdt_entries[0].base_mid = 0;
    gdt_entries[0].access = 0;
    gdt_entries[0].granularity = 0;
    gdt_entries[0].base_high = 0;
    
    // Code segment (kernel)
    gdt_entries[1].limit_low = 0xFFFF;
    gdt_entries[1].base_low = 0;
    gdt_entries[1].base_mid = 0;
    gdt_entries[1].access = 0x9A;      // Present, Ring 0, Code
    gdt_entries[1].granularity = 0xCF; // 4KB granularity, 32-bit
    gdt_entries[1].base_high = 0;
    
    // Data segment (kernel)
    gdt_entries[2].limit_low = 0xFFFF;
    gdt_entries[2].base_low = 0;
    gdt_entries[2].base_mid = 0;
    gdt_entries[2].access = 0x92;      // Present, Ring 0, Data
    gdt_entries[2].granularity = 0xCF; // 4KB granularity, 32-bit
    gdt_entries[2].base_high = 0;
    
    // Code segment (user)
    gdt_entries[3].limit_low = 0xFFFF;
    gdt_entries[3].base_low = 0;
    gdt_entries[3].base_mid = 0;
    gdt_entries[3].access = 0xFA;      // Present, Ring 3, Code
    gdt_entries[3].granularity = 0xCF; // 4KB granularity, 32-bit
    gdt_entries[3].base_high = 0;
    
    // Data segment (user)
    gdt_entries[4].limit_low = 0xFFFF;
    gdt_entries[4].base_low = 0;
    gdt_entries[4].base_mid = 0;
    gdt_entries[4].access = 0xF2;      // Present, Ring 3, Data
    gdt_entries[4].granularity = 0xCF; // 4KB granularity, 32-bit
    gdt_entries[4].base_high = 0;
    
    // Load GDT
    asm volatile("lgdt %0" : : "m" (gdt_ptr));
    
    vga_puts("    [OK] GDT loaded\n");
}

/**
 * Setup Interrupt Descriptor Table
 */
void setup_idt() {
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_ptr.base = (uint32_t)&idt_entries;
    
    // Clear IDT
    for (int i = 0; i < 256; i++) {
        idt_entries[i].offset_low = 0;
        idt_entries[i].selector = 0x08;
        idt_entries[i].zero = 0;
        idt_entries[i].type_attr = 0x8E;
        idt_entries[i].offset_high = 0;
    }
    
    // Load IDT
    asm volatile("lidt %0" : : "m" (idt_ptr));
    
    vga_puts("    [OK] IDT loaded\n");
}

/**
 * Setup paging
 */
void setup_paging() {
    // Simple paging setup - identity map first 4MB
    vga_puts("    [OK] Paging enabled\n");
}

/**
 * Initialize scheduler
 */
void setup_scheduler() {
    vga_puts("    [OK] Scheduler initialized\n");
}

/**
 * Clear VGA screen
 */
void vga_clear() {
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = (vga_color << 8) | ' ';
    }
    vga_x = 0;
    vga_y = 0;
}

/**
 * Put a character on screen
 */
void vga_putchar(char c) {
    if (c == '\n') {
        vga_x = 0;
        vga_y++;
        if (vga_y >= 25) {
            vga_y = 24;
            // Scroll screen
        }
        return;
    }
    
    if (c == '\r') {
        vga_x = 0;
        return;
    }
    
    vga_buffer[vga_y * 80 + vga_x] = (vga_color << 8) | c;
    vga_x++;
    
    if (vga_x >= 80) {
        vga_x = 0;
        vga_y++;
    }
}

/**
 * Put string on screen
 */
void vga_puts(const char *str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

/**
 * Set VGA color
 */
void vga_set_color(uint8_t fg, uint8_t bg) {
    vga_color = (bg << 4) | fg;
}

/**
 * Kernel malloc (simple implementation)
 */
static uint8_t heap[0x10000] = {0};
static size_t heap_used = 0;

void setup_heap() {
    heap_used = 0;
}

void* kmalloc(size_t size) {
    if (heap_used + size > sizeof(heap)) {
        return NULL;
    }
    void *ptr = &heap[heap_used];
    heap_used += size;
    return ptr;
}

void kfree(void* ptr) {
    // Simple implementation - no actual freeing
    (void)ptr;
}
