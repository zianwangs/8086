; ORG 0x7c00
BITS 16

global entry
extern load_kernel

SECTION .text align=16
entry:
    lgdt [gdt_desc]
    in al, 0x92
    or al, 0000_0010B
    out 0x92, al
    mov ax, 0
    call load_kernel
    call reachable
    jmp $
    cli
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp dword 0x0008:flush

BITS 32
flush:
    mov cx, 0x10
    mov ds, cx
    mov eax, 0
    mov edi, 0
    mov esi, 0
    jmp $

reachable:
    add al, 0x30
    mov ah, 0xE
    int 0x10
    ret

gdt: dq 0
     dq 0x00c098000000ffff
     dq 0x00c092000000ffff
gdt_desc:
    .gdt_size: dw gdt_desc - gdt - 1
    .gdt_base: dd gdt





