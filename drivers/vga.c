// Windows XP 2010 Edition - VGA Driver
// drivers/vga.c

#include "kernel.h"

static uint16_t *vga_buffer = (uint16_t *)0xB8000;
static uint8_t vga_x = 0, vga_y = 0;
static uint8_t vga_fg = 15, vga_bg = 0;  // White on black

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/**
 * Initialize VGA driver
 */
void vga_init() {
    vga_x = 0;
    vga_y = 0;
    vga_fg = 15;
    vga_bg = 0;
}

/**
 * Clear screen
 */
void vga_clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = ((vga_bg << 4) | vga_fg) << 8 | ' ';
    }
    vga_x = 0;
    vga_y = 0;
}

/**
 * Scroll screen up
 */
static void vga_scroll() {
    for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
        vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
    }
    
    for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = ((vga_bg << 4) | vga_fg) << 8 | ' ';
    }
    
    vga_y--;
}

/**
 * Put character on screen
 */
void vga_putchar(char c) {
    if (c == '\n') {
        vga_x = 0;
        vga_y++;
        if (vga_y >= VGA_HEIGHT) {
            vga_scroll();
        }
        return;
    }
    
    if (c == '\r') {
        vga_x = 0;
        return;
    }
    
    if (c == '\t') {
        vga_x += 4;
        if (vga_x >= VGA_WIDTH) {
            vga_x = 0;
            vga_y++;
            if (vga_y >= VGA_HEIGHT) {
                vga_scroll();
            }
        }
        return;
    }
    
    vga_buffer[vga_y * VGA_WIDTH + vga_x] = 
        ((vga_bg << 4) | vga_fg) << 8 | (uint8_t)c;
    
    vga_x++;
    if (vga_x >= VGA_WIDTH) {
        vga_x = 0;
        vga_y++;
        if (vga_y >= VGA_HEIGHT) {
            vga_scroll();
        }
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
 * Put integer as hex
 */
void vga_puthex(uint32_t val) {
    const char *hex = "0123456789ABCDEF";
    vga_putchar('0');
    vga_putchar('x');
    
    for (int i = 28; i >= 0; i -= 4) {
        vga_putchar(hex[(val >> i) & 0xF]);
    }
}

/**
 * Put integer as decimal
 */
void vga_putdec(uint32_t val) {
    if (val == 0) {
        vga_putchar('0');
        return;
    }
    
    char buf[32];
    int i = 0;
    
    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    
    while (i > 0) {
        vga_putchar(buf[--i]);
    }
}

/**
 * Set foreground color
 */
void vga_set_fg(uint8_t color) {
    vga_fg = color & 0x0F;
}

/**
 * Set background color
 */
void vga_set_bg(uint8_t color) {
    vga_bg = color & 0x0F;
}

/**
 * Get cursor position
 */
void vga_get_cursor(uint8_t *x, uint8_t *y) {
    *x = vga_x;
    *y = vga_y;
}

/**
 * Set cursor position
 */
void vga_set_cursor(uint8_t x, uint8_t y) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        vga_x = x;
        vga_y = y;
    }
}

// VGA color codes
#define VGA_BLACK       0
#define VGA_BLUE        1
#define VGA_GREEN       2
#define VGA_CYAN        3
#define VGA_RED         4
#define VGA_MAGENTA     5
#define VGA_BROWN       6
#define VGA_LIGHT_GRAY  7
#define VGA_DARK_GRAY   8
#define VGA_LIGHT_BLUE  9
#define VGA_LIGHT_GREEN 10
#define VGA_LIGHT_CYAN  11
#define VGA_LIGHT_RED   12
#define VGA_LIGHT_MAG   13
#define VGA_YELLOW      14
#define VGA_WHITE       15
