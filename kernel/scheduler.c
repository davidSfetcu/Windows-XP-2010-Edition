// Windows XP 2010 Edition - Scheduler
// kernel/scheduler.c

#include "kernel.h"

// Process table
typedef struct {
    uint32_t pid;
    uint32_t state;  // 0=ready, 1=running, 2=blocked, 3=terminated
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    uint32_t priority;
    uint32_t time_slice;
    char name[32];
} task_t;

#define MAX_TASKS 256
static task_t task_table[MAX_TASKS];
static uint32_t task_count = 0;
static uint32_t current_task = 0;

/**
 * Create a new process
 */
uint32_t create_process(const char *name, uint32_t entry_point, uint32_t priority) {
    if (task_count >= MAX_TASKS) {
        return 0;
    }
    
    task_t *task = &task_table[task_count];
    
    task->pid = task_count;
    task->state = 0;  // Ready
    task->esp = 0x90000 - (task_count * 0x10000);
    task->ebp = task->esp;
    task->eip = entry_point;
    task->priority = priority;
    task->time_slice = 100;
    
    // Copy name
    int i = 0;
    while (name[i] && i < 31) {
        task->name[i] = name[i];
        i++;
    }
    task->name[i] = '\0';
    
    task_count++;
    return task->pid;
}

/**
 * Terminate process
 */
void terminate_process(uint32_t pid) {
    if (pid < task_count) {
        task_table[pid].state = 3;  // Terminated
    }
}

/**
 * Get current process
 */
uint32_t get_current_process() {
    return current_task;
}

/**
 * Switch process (context switch)
 */
void schedule() {
    // Simple round-robin scheduler
    do {
        current_task++;
        if (current_task >= task_count) {
            current_task = 0;
        }
    } while (task_table[current_task].state != 0 && 
             task_table[current_task].state != 1);
}

/**
 * Get process info
 */
void get_process_info(uint32_t pid, char *buf, size_t len) {
    if (pid >= task_count) {
        return;
    }
    
    task_t *task = &task_table[pid];
    
    // Format: PID NAME STATE
    int pos = 0;
    
    // Add PID
    if (pid < 10) {
        buf[pos++] = '0' + pid;
    } else {
        buf[pos++] = '0' + (pid / 10);
        buf[pos++] = '0' + (pid % 10);
    }
    
    buf[pos++] = ' ';
    
    // Add name
    for (int i = 0; task->name[i] && pos < len - 10; i++) {
        buf[pos++] = task->name[i];
    }
    
    buf[pos++] = ' ';
    
    // Add state
    const char *state_str;
    switch (task->state) {
        case 0: state_str = "READY"; break;
        case 1: state_str = "RUNNING"; break;
        case 2: state_str = "BLOCKED"; break;
        case 3: state_str = "TERMINATED"; break;
        default: state_str = "UNKNOWN"; break;
    }
    
    for (int i = 0; state_str[i] && pos < len - 1; i++) {
        buf[pos++] = state_str[i];
    }
    
    buf[pos] = '\0';
}

/**
 * List all processes
 */
void list_processes() {
    vga_puts("PID  NAME                 STATE\n");
    vga_puts("---  ----                 -----\n");
    
    for (uint32_t i = 0; i < task_count; i++) {
        char buf[64];
        get_process_info(i, buf, sizeof(buf));
        vga_puts(buf);
        vga_puts("\n");
    }
}

/**
 * Get task count
 */
uint32_t get_task_count() {
    return task_count;
}
