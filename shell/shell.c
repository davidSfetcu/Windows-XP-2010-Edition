// Windows XP 2010 Edition - Shell
// shell/shell.c

#include "kernel.h"

#define CMD_BUFFER_SIZE 256
#define MAX_ARGS 16

// Built-in commands
typedef struct {
    char name[32];
    void (*func)(int argc, char **argv);
} command_t;

/**
 * Help command
 */
static void cmd_help(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    vga_puts("Available commands:\n");
    vga_puts("  help     - Show this help\n");
    vga_puts("  clear    - Clear screen\n");
    vga_puts("  echo     - Echo arguments\n");
    vga_puts("  ps       - List processes\n");
    vga_puts("  reboot   - Reboot system\n");
    vga_puts("  malloc   - Test memory allocation\n");
    vga_puts("  info     - System information\n");
}

/**
 * Clear command
 */
static void cmd_clear(int argc, char **argv) {
    (void)argc;
    (void)argv;
    vga_clear();
}

/**
 * Echo command
 */
static void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        vga_puts(argv[i]);
        if (i < argc - 1) {
            vga_putchar(' ');
        }
    }
    vga_putchar('\n');
}

/**
 * Process list command
 */
static void cmd_ps(int argc, char **argv) {
    (void)argc;
    (void)argv;
    list_processes();
}

/**
 * Reboot command
 */
static void cmd_reboot(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    vga_puts("Rebooting...\n");
    asm volatile("jmp 0xFFFF:0x0000");
}

/**
 * Memory test command
 */
static void cmd_malloc(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    vga_puts("Memory allocation test:\n");
    
    void *p1 = kmalloc(1024);
    void *p2 = kmalloc(2048);
    void *p3 = kmalloc(512);
    
    vga_puts("  Allocated 1KB at ");
    vga_puthex((uint32_t)p1);
    vga_puts("\n");
    
    vga_puts("  Allocated 2KB at ");
    vga_puthex((uint32_t)p2);
    vga_puts("\n");
    
    vga_puts("  Allocated 512B at ");
    vga_puthex((uint32_t)p3);
    vga_puts("\n");
}

/**
 * System info command
 */
static void cmd_info(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    vga_puts("=== System Information ===\n");
    vga_puts("OS: Windows XP 2010 Edition\n");
    vga_puts("Kernel Version: 1.0\n");
    vga_puts("Memory: 128 MB\n");
    vga_puts("CPU: Intel 80386+\n");
    vga_puts("Running Processes: ");
    vga_putdec(get_task_count());
    vga_puts("\n");
}

// Command table
static command_t commands[] = {
    {"help", cmd_help},
    {"clear", cmd_clear},
    {"echo", cmd_echo},
    {"ps", cmd_ps},
    {"reboot", cmd_reboot},
    {"malloc", cmd_malloc},
    {"info", cmd_info},
    {"", NULL}
};

/**
 * Parse command line
 */
static void parse_command(const char *line, int *argc, char **argv) {
    *argc = 0;
    const char *p = line;
    char *arg_start = NULL;
    
    static char arg_buffer[CMD_BUFFER_SIZE];
    char *buf_pos = arg_buffer;
    
    while (*p && *argc < MAX_ARGS) {
        if (*p == ' ' || *p == '\t') {
            if (arg_start) {
                // End current argument
                *buf_pos = '\0';
                argv[*argc] = arg_start;
                (*argc)++;
                arg_start = NULL;
                buf_pos = arg_buffer;
            }
            p++;
        } else {
            if (!arg_start) {
                arg_start = buf_pos;
            }
            *buf_pos = *p;
            buf_pos++;
            p++;
        }
    }
    
    if (arg_start) {
        *buf_pos = '\0';
        argv[*argc] = arg_start;
        (*argc)++;
    }
}

/**
 * Execute command
 */
static void execute_command(const char *line) {
    int argc;
    char *argv[MAX_ARGS];
    
    parse_command(line, &argc, argv);
    
    if (argc == 0) {
        return;
    }
    
    // Search for command
    for (int i = 0; commands[i].func != NULL; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].func(argc, argv);
            return;
        }
    }
    
    vga_puts("Unknown command: ");
    vga_puts(argv[0]);
    vga_puts("\n");
}

/**
 * String compare
 */
static int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

/**
 * Main shell loop
 */
void shell_main() {
    static char cmd_line[CMD_BUFFER_SIZE];
    
    vga_set_fg(10);  // Green
    
    while (1) {
        vga_puts("xp2010> ");
        vga_set_fg(15);  // White
        
        keyboard_gets(cmd_line, sizeof(cmd_line));
        
        vga_set_fg(10);  // Green
        
        if (cmd_line[0] != '\0') {
            execute_command(cmd_line);
        }
        
        vga_puts("\n");
    }
}
