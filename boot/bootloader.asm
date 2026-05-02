; Windows XP 2010 Edition - Bootloader
; x86-16 Real Mode Bootloader
; Loads kernel from disk and switches to Protected Mode

[BITS 16]
[ORG 0x7C00]

; Boot sector entry point
boot_start:
    cli                          ; Disable interrupts
    cld                          ; Clear direction flag
    
    ; Set up stack
    mov ax, 0x7C0
    mov ss, ax
    mov sp, 0x4000
    
    ; Set up data segment
    mov ds, ax
    mov es, ax
    
    ; Clear screen
    mov ax, 0x0003
    int 0x10
    
    ; Print boot message
    mov si, boot_msg
    call print_string
    
    ; Enable A20 line (legacy requirement)
    call enable_a20
    
    ; Load GDT
    lgdt [gdt_descriptor]
    
    ; Load kernel from disk
    mov si, kernel_msg
    call print_string
    call load_kernel
    
    ; Switch to Protected Mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Far jump to Protected Mode
    jmp 0x0008:protected_mode
    
; Enable A20 line
enable_a20:
    in al, 0x92
    or al, 0x02
    out 0x92, al
    ret
    
; Load kernel from disk (LBA)
load_kernel:
    mov ax, 0x0201      ; Read 1 sector
    mov bx, 0x1000      ; Load to 0x1000
    mov cx, 0x0002      ; Sector 2
    mov dx, 0x0000      ; Drive 0
    int 0x13
    
    jc disk_error
    ret
    
disk_error:
    mov si, error_msg
    call print_string
    hlt
    
; Print string (null-terminated)
print_string:
    lodsb
    cmp al, 0
    je .done
    mov ah, 0x0E
    mov bh, 0x00
    int 0x10
    jmp print_string
.done:
    ret

; GDT (Global Descriptor Table)
gdt:
    ; Null descriptor
    dq 0x0000000000000000
    
    ; Code segment (0x0008)
    dq 0x00CF9A000000FFFF
    
    ; Data segment (0x0010)
    dq 0x00CF92000000FFFF

gdt_descriptor:
    dw gdt_end - gdt - 1
    dd gdt

gdt_end:

; Messages
boot_msg: db "Windows XP 2010 - Booting...", 0x0D, 0x0A, 0
kernel_msg: db "Loading kernel...", 0x0D, 0x0A, 0
error_msg: db "Disk error!", 0x0D, 0x0A, 0

; Padding to 512 bytes
times 510 - ($ - $$) db 0
dw 0xAA55  ; Boot signature

; Protected Mode code
[BITS 32]

protected_mode:
    ; Set up segment registers
    mov ax, 0x0010
    mov ds, ax
    mov es, ax
    mov ss, ax
    
    ; Set up stack in high memory
    mov esp, 0x90000
    
    ; Jump to kernel
    jmp 0x1000
