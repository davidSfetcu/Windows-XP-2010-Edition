// Windows XP 2010 Edition - Keyboard Driver
// drivers/keyboard.c

#include "kernel.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Keyboard input buffer
#define KB_BUFFER_SIZE 256
static char kb_buffer[KB_BUFFER_SIZE];
static uint32_t kb_head = 0;
static uint32_t kb_tail = 0;

// Scancode to ASCII conversion table
static const char scancode_table[128] = {
    0, 27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', 8, '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', 13, 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    39, '`', 0, 92, 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, 0,
    0, ' ', 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * Initialize keyboard driver
 */
void keyboard_init() {
    kb_head = 0;
    kb_tail = 0;
}

/**
 * Read from keyboard port
 */
static uint8_t kb_read_data() {
    while ((inb(KEYBOARD_STATUS_PORT) & 1) == 0);
    return inb(KEYBOARD_DATA_PORT);
}

/**
 * Write to keyboard port
 */
static void kb_write_data(uint8_t data) {
    while ((inb(KEYBOARD_STATUS_PORT) & 2) != 0);
    outb(KEYBOARD_DATA_PORT, data);
}

/**
 * Keyboard interrupt handler
 */
void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    if (scancode < 128) {
        char c = scancode_table[scancode];
        if (c != 0) {
            // Add to buffer
            kb_buffer[kb_tail] = c;
            kb_tail = (kb_tail + 1) % KB_BUFFER_SIZE;
        }
    }
}

/**
 * Check if key is available
 */
uint32_t keyboard_available() {
    return kb_head != kb_tail;
}

/**
 * Get key from buffer
 */
char keyboard_getchar() {
    while (!keyboard_available()) {
        asm volatile("hlt");
    }
    
    char c = kb_buffer[kb_head];
    kb_head = (kb_head + 1) % KB_BUFFER_SIZE;
    return c;
}

/**
 * Get string from keyboard
 */
void keyboard_gets(char *buffer, size_t max_len) {
    size_t i = 0;
    while (i < max_len - 1) {
        char c = keyboard_getchar();
        
        if (c == '\n' || c == 13) {
            buffer[i] = '\0';
            vga_putchar('\n');
            break;
        } else if (c == 8) {
            // Backspace
            if (i > 0) {
                i--;
                vga_putchar(8);
                vga_putchar(' ');
                vga_putchar(8);
            }
        } else {
            buffer[i] = c;
            vga_putchar(c);
            i++;
        }
    }
    
    if (i >= max_len - 1) {
        buffer[max_len - 1] = '\0';
    }
}

/**
 * I/O port functions
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1,%0" : "=a" (ret) : "dN" (port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %0,%1" : : "a" (data), "dN" (port));
}
