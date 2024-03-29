BITS 16
;-------------------------
global entry
; extern load_kernel
;-------------------------
; KERNEL_BLOCKS      from Makefile
;-------------------------
disk_sector_size     equ 0x200
;-------------------------
dword_size           equ 0x4
pg_entry_size        equ 0x8
pg_size              equ 0x1000
pg_writable          equ 0x2
pg_present           equ 0x1
pg_perm_bits         equ 0x3
pg_entry_index_bits  equ 0x9
pg_entry_offset_bits equ 0xC
pg_entry_index_mask  equ 0x1FF
pg_table_levels      equ 0x4
;-------------------------
kpgdir_pa            equ 0x100000
kernel_pa_start      equ 0x200000
; boot_sector_pa_start equ 0x7000
; boot_sector_pa_end   equ 0x8000
; ioapic_pa_start      equ 0xFEC00000
; ioapic_pa_end        equ 0xFEC01000
; lapic_pa_start       equ 0xFEE00000
; lapic_pa_end         equ 0xFEE01000
;-------------------------
boot_sector_va_start equ 0x7000
boot_sector_va_end   equ 0x8000
kernel_va_start      equ 0x80000000
kernel_va_end        equ 0x80100000  ; 1MB
ioapic_va_start      equ 0xFEC00000
ioapic_va_end        equ 0xFEC01000
lapic_va_start       equ 0xFEE00000
lapic_va_end         equ 0xFEE01000
; kpgdir_va            equ 0xFFFFF000
;-------------------------
SECTION .text align=16
entry:
    cli
    lgdt [gdt_desc]
   
    in al, 0x92
    or al, 0000_0010B
    out 0x92, al
  
    mov esp, boot_sector_va_end

    ; mov edi, 0x100000
    ; .clear:
    ; mov dword [edi], 0
    ; add edi, 4
    ; cmp edi, 0x300000
    ; jb .clear

    call .alloc_one_clear_page ; for kpgdir
    ; call .alloc_one_clear_page

    mov edi, boot_sector_va_start 
    mov esi, boot_sector_va_start
    mov dx, (boot_sector_va_end - boot_sector_va_start) / pg_size
    call .map_pages

    ; mov dword [dword 0x100000], 0x101003
    ; mov dword [dword 0x101000], 0x102003
    ; mov dword [dword 0x102000], 0x103003
    ; mov dword [dword 0x103000], 0x0003
    ; mov dword [dword 0x103008], 0x1003 
    ; mov dword [dword 0x103010], 0x2003 
    ; mov dword [dword 0x103018], 0x3003 
    ; mov dword [dword 0x103020], 0x4003
    ; mov dword [dword 0x103028], 0x5003 
    ; mov dword [dword 0x103030], 0x6003 
    ; mov dword [dword 0x103038], 0x7003
    ; mov dword [dword 0x103040], 0x8003
    ; mov dword [dword 0x103072], 0x9003
    ; mov dword [dword 0x103080], 0x10003
    ; mov edi, 0x103000
    ; mov eax, 0
    ; .ll:
    ; mov ebx, eax
    ; or ebx, 3
    ; mov [edi + eax * 8], ebx
    ; inc eax
    ; cmp eax, 10
    ; jne .ll

    mov edi, kernel_va_start
    mov esi, kernel_pa_start
    mov dx, (kernel_va_end - kernel_va_start) / pg_size
    call .map_pages

    mov edi, ioapic_va_start
    mov esi, ioapic_va_start
    mov dx, (ioapic_va_end - ioapic_va_start) / pg_size
    call .map_pages

    mov edi, lapic_va_start
    mov esi, lapic_va_start
    mov dx, (lapic_va_end - lapic_va_start) / pg_size
    call .map_pages

    mov edi, kpgdir_pa
    mov esi, kpgdir_pa
    mov dx, (kernel_va_start - kpgdir_pa) / pg_size
    call .map_pages

    ; page table setup is complete
   
    mov eax, 10100000b      ; Set the PAE and PGE bit.
    mov cr4, eax
    
    mov edx, kpgdir_pa     ; Point CR3 at the PML4.
    mov cr3, edx
 
    mov ecx, 0xC0000080     ; Read from the EFER MSR. 
    rdmsr    
    or eax, 0x00000100      ; Set the LME bit.
    wrmsr
    
    mov ebx, cr0            ; Activate long mode -
    or ebx, 0x80000001      ; - by enabling paging and protection simultaneously.
    mov cr0, ebx
    
    jmp 0x0008:long_mode

    .alloc_one_clear_page: ; void alloc_one_clear_page(edi:specified_pa = 0);
        cld
        mov eax, [next_physical_page_addr] ; no specified pa, use global allocator
        xor ebp, ebp
        .memset:
            mov dword [eax + ebp * 4], 0
            inc bp
            cmp bp, pg_size / dword_size
            jne .memset
        lea ebp, [eax + pg_size]
        mov [next_physical_page_addr], ebp
        ret

    .map_pages: ; void mappages(esi:va_start, esi:specified_pa_start=0, dx:pages)
        .map_one_page:
            dec dx
            cmp dx, 0xFFFF
            je .end
            mov ebp, kpgdir_pa ; no other registers :)
            mov ebx, pg_table_levels
            mov cl, 0x27 ; 39           
            .map_one_level:
                xor eax, eax
                cmp cl, 0x1f
                ja .exceeds_31_bits
                mov eax, edi
                shr eax, cl        
                and eax, 0x1ff
                .exceeds_31_bits:
                    ; nop
                lea ebp, [ebp + eax * 8]
                cmp ebx, 0x1
                jne .not_the_last_level
                test esi, esi
                je .not_the_last_level
                mov eax, esi
                add esi, pg_size
                or eax, pg_perm_bits
                mov [ebp], eax
                jmp .exists
                .not_the_last_level:
                    cmp dword [ebp], 0
                    jne .exists
                push ebp
                call .alloc_one_clear_page
                pop ebp
                or eax, pg_perm_bits
                mov [ebp], eax
                .exists:
                    mov ebp, [ebp]
                    and ebp, 0xFFFFF000
                sub cl, 0x9
                dec ebx
                jne .map_one_level
                add edi, pg_size
                jmp .map_one_page
                .end:
                    ret


next_physical_page_addr: dd 0x100000

gdt: dq 0
     dq 0x00209A0000000000 ; code L, P, S, E, RW
     dq 0x0000920000000000 ; data P, S, RW(not needed)

gdt_desc:
    .gdt_size: dw gdt_desc - gdt - 1
    .gdt_base: dd gdt

; reachable:
;     push eax
;     mov al, [data]
;     cmp al, 0x40
;     je .gg
;     mov ah, 0xe
;     int 0x10
;     inc al
;     mov [data], al
;     pop eax
; .gg:
;     ret
    
;     ; jmp $
; data: db 0x30
;-------------------------
BITS 64
;-------------------------
long_mode:
    ; mov ax, 0x0010
    ; mov ds, ax
    ; mov es, ax
    ; mov fs, ax
    ; mov gs, ax
    ; mov ss, ax
    ; looks like segs other than cs are not required

    mov bl, KERNEL_BLOCKS ; max(0 = 256 = 128kb)

    mov dx, 0x1f2
    mov al, bl 
    out dx, al     ; how many sectors to read

    inc dx
    mov al, 1
    out dx, al     ;LBA address 7~0

    inc dx
    mov al, 0
    out dx, al     ;LBA address 15~8

    inc dx
    mov al, 0
    out dx, al     ;LBA address 23~16

    inc dx
    mov al, 0xe0   ;LBA28 mode，master drive
    or al, 0       ;LBA address 27~24
    out dx, al

    inc dx
    mov al, 0x20
    out dx, al     ; read

    mov dx, 0x1f7  ; status port
    mov edi, kernel_va_start
    
    .disk_busy:
        in al, dx
        and al, 0x88   ; or (al & 0xC0) != 0x40 | (al & 0x88) != 0x40
        cmp al, 0x8 
        jne .disk_busy 

    mov dx, 0x1f0
    mov rcx, disk_sector_size / dword_size ; how many dwords to read

    .read_dword:
        in eax, dx
        mov [edi], eax
        add edi, 4
        loop .read_dword
    
    or dl, 7  ; back to status port
    in al, dx
    in al, dx
    in al, dx
    in al, dx ; 400 ns delay

    dec bl
    cmp bl, 0
    jne .disk_busy
    
    mov rbp, kernel_va_end
    mov rsp, kernel_va_end
    mov rax, kernel_va_start

    jmp rax ; to the kernel entrypoint 'main', never returns

    ; raise a divided by 0 exception to know we're back
    mov bl, 0
    div bl




