// Windows XP 2010 Edition - Memory Manager
// memory.c

#include "kernel.h"

#define PAGE_DIR_SIZE 1024
#define PAGE_TABLE_SIZE 1024

// Page directory and tables
uint32_t page_directory[PAGE_DIR_SIZE] __attribute__((aligned(4096)));
uint32_t page_tables[10][PAGE_TABLE_SIZE] __attribute__((aligned(4096)));

/**
 * Initialize paging
 */
void init_paging() {
    // Clear page directory
    for (int i = 0; i < PAGE_DIR_SIZE; i++) {
        page_directory[i] = 0x00000002; // Supervisor, read/write, not present
    }
    
    // Create page tables
    for (int t = 0; t < 10; t++) {
        for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
            page_tables[t][i] = (t * PAGE_TABLE_SIZE + i) * PAGE_SIZE | 3; // Present, read/write
        }
        page_directory[t] = (uint32_t)&page_tables[t] | 3;
    }
    
    // Load page directory
    asm volatile("mov %0, %%cr3" : : "r"(page_directory));
    
    // Enable paging
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

/**
 * Map a physical page to virtual address
 */
void map_page(uint32_t virt, uint32_t phys) {
    uint32_t dir_idx = virt >> 22;
    uint32_t table_idx = (virt >> 12) & 0x3FF;
    
    if (!(page_directory[dir_idx] & 1)) {
        // Allocate new table
        page_directory[dir_idx] = phys | 3;
    }
    
    uint32_t *table = (uint32_t *)(page_directory[dir_idx] & 0xFFFFF000);
    table[table_idx] = phys | 3;
}

/**
 * Unmap a virtual page
 */
void unmap_page(uint32_t virt) {
    uint32_t dir_idx = virt >> 22;
    uint32_t table_idx = (virt >> 12) & 0x3FF;
    
    if (page_directory[dir_idx] & 1) {
        uint32_t *table = (uint32_t *)(page_directory[dir_idx] & 0xFFFFF000);
        table[table_idx] = 0;
    }
}

/**
 * Get physical address from virtual address
 */
uint32_t get_physical_address(uint32_t virt) {
    uint32_t dir_idx = virt >> 22;
    uint32_t table_idx = (virt >> 12) & 0x3FF;
    
    if (!(page_directory[dir_idx] & 1)) {
        return 0;
    }
    
    uint32_t *table = (uint32_t *)(page_directory[dir_idx] & 0xFFFFF000);
    if (!(table[table_idx] & 1)) {
        return 0;
    }
    
    return (table[table_idx] & 0xFFFFF000) + (virt & 0xFFF);
}

/**
 * Memory allocation tracker
 */
typedef struct {
    void *ptr;
    size_t size;
    uint32_t alloced;
} mem_block_t;

#define MAX_MEM_BLOCKS 1024
static mem_block_t mem_blocks[MAX_MEM_BLOCKS];
static size_t mem_block_count = 0;

/**
 * Allocate memory
 */
void* malloc_aligned(size_t size, size_t align) {
    if (mem_block_count >= MAX_MEM_BLOCKS) {
        return NULL;
    }
    
    // Simple allocation - just find free memory
    void *ptr = (void *)0x00110000 + (mem_block_count * align);
    
    mem_blocks[mem_block_count].ptr = ptr;
    mem_blocks[mem_block_count].size = size;
    mem_blocks[mem_block_count].alloced = 1;
    mem_block_count++;
    
    return ptr;
}

/**
 * Free memory
 */
void free_aligned(void *ptr) {
    for (size_t i = 0; i < mem_block_count; i++) {
        if (mem_blocks[i].ptr == ptr) {
            mem_blocks[i].alloced = 0;
            return;
        }
    }
}

/**
 * Memset
 */
void* memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;
    for (size_t i = 0; i < n; i++) {
        p[i] = c;
    }
    return s;
}

/**
 * Memcpy
 */
void* memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}
