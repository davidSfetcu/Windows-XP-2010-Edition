# Build configuration for Windows XP 2010 Edition

CC = i686-elf-gcc
AS = nasm
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

CFLAGS = -ffreestanding -fno-stack-protector -nostdlib -m32 -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386

# Source files
BOOT_SRC = boot/bootloader.asm
KERNEL_SRCS = kernel/kernel.c kernel/memory.c kernel/scheduler.c
DRIVER_SRCS = drivers/vga.c drivers/keyboard.c
SHELL_SRCS = shell/shell.c

# Object files
BOOT_OBJ = build/bootloader.o
KERNEL_OBJS = $(KERNEL_SRCS:%.c=build/%.o)
DRIVER_OBJS = $(DRIVER_SRCS:%.c=build/%.o)
SHELL_OBJS = $(SHELL_SRCS:%.c=build/%.o)

# Final targets
KERNEL_BIN = build/kernel.bin
ISO = windows_xp_2010.iso

.PHONY: all clean iso

all: $(KERNEL_BIN)

$(BOOT_OBJ): $(BOOT_SRC)
	@mkdir -p build/boot
	$(AS) $(ASFLAGS) -o $@ $<

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(KERNEL_BIN): $(BOOT_OBJ) $(KERNEL_OBJS) $(DRIVER_OBJS) $(SHELL_OBJS)
	$(LD) $(LDFLAGS) -T linker.ld -o build/kernel.elf $^
	$(OBJCOPY) -O binary build/kernel.elf $@

iso: $(KERNEL_BIN)
	@echo "Creating ISO image..."
	@mkdir -p isodir/boot
	@cp $(KERNEL_BIN) isodir/boot/
	@xorriso -as mkisofs -R -J -c isodir/boot/boot.cat \
		-b boot/$(notdir $(KERNEL_BIN)) \
		-no-emul-boot -boot-load-size 4 \
		-input-charset utf-8 -output $(ISO) isodir/ 2>/dev/null || \
	mkisofs -R -J -c boot/boot.cat \
		-b boot/$(notdir $(KERNEL_BIN)) \
		-no-emul-boot -boot-load-size 4 \
		-o $(ISO) isodir/

clean:
	rm -rf build isodir
	rm -f $(ISO)

.PRECIOUS: build/%.o
