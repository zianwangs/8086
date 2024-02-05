; ORG 0x7c00
BITS 16

SECTION header align=16 vstart=0x7c00
    mov dx, 0x1f2
    mov al, 1
    out dx, al                       ;读取的扇区数

    inc dx                          ;0x1f3
    mov al, 1
    out dx, al                       ;LBA地址7~0

    inc dx                          ;0x1f4
    mov al, 0
    out dx, al                       ;LBA地址15~8

    inc dx                          ;0x1f5
    mov ax, 0
    out dx, al                       ;LBA地址23~16

    inc dx                          ;0x1f6
    mov al, 0xe0                     ;LBA28模式，主盘
    or al, 0                        ;LBA地址27~24
    out dx, al

    inc dx                          ;0x1f7
    mov al, 0x20                     ;读命令
    out dx, al

.wait:
    in al, dx
    and al, 0x88   ; or & 0xC0 != 0x40
    cmp al, 0x8
    jne .wait                      ;不忙，且硬盘已准备好数据传输 

    mov cx, 256                     ;总共要读取的字数
    mov dx, 0x1f0
    mov ah, 0xE
.readb:
    in ax, dx
    cmp al, 0
    je end
    mov bh, ah
    mov ah, 0xE
    int 0x10
    cmp bh, 0
    je end
    mov al, bh
    int 0x10
    loop .readb


end:
    mov al, 0
    mov ah, 0
    int 0x16
    call adjust
    mov ah, 0xe
    int 0x10
    jmp end
  
adjust:
    cmp al, 0x7F
    je is_delete
    cmp al, 0x8
    jne not_delete
is_delete:
    mov al, 0x08
    mov ah, 0xE
    int 0x10
    mov al, 0x20
    mov ah, 0xE
    int 0x10
    mov al, 0x08
not_delete:
    ret


reachable:
    mov [0x10000], ax
    mov al, 0x30
    mov ah, 0xE
    int 0x10
    mov ax, [0x10000]
    ret


number: db 123, 0
times 510-($-$$) db 0
dw 0xAA55