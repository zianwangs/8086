BITS 32
;-------------------------
global entry
extern load_kernel
;-------------------------
dword_size           equ 0x4
pg_entry_size        equ 0x8
pg_size              equ 0x1000
pg_writable          equ 0x10
pg_present           equ 0x01
pg_perm_bits         equ 0x11
pg_entry_index_bits  equ 0x9
pg_entry_offset_bits equ 0xC
pg_entry_index_mask  equ 0x1FF
pg_table_level       equ 0x4
;-------------------------
kpgdir_pa            equ 0x100000
boot_sector_pa_start equ 0x7000
boot_sector_pa_end   equ 0x8000
ioapic_pa_start      equ 0xFEC00000
ioapic_pa_end        equ 0xFEC01000
lapic_pa_start       equ 0xFEE00000
lapic_pa_end         equ 0xFEE01000
;-------------------------
boot_sector_va_start equ 0x7000
boot_sector_va_end   equ 0x8000
kernel_va_start      equ 0x80000000
kernel_va_end        equ 0x80010000
ioapic_va_start      equ 0xFEC00000
ioapic_va_end        equ 0xFEC01000
lapic_va_start       equ 0xFEE00000
lapic_va_end         equ 0xFEE01000
kpgdir_va            equ 0xFFFFF000
;-------------------------
SECTION .text align=16
entry:
    cli

    lgdt [gdt_desc]
    in al, 0x92
    or al, 0000_0010B
    out 0x92, al
    
    mov esp, boot_sector_pa_end
  
    call .alloc_one_clear_page ; for kpgdir
    ; call reachable
    mov edi, boot_sector_va_start 
    mov esi, boot_sector_pa_start
    mov edx, (boot_sector_va_end - boot_sector_va_start) / pg_size
    call .map_pages
    
    mov edi, kernel_va_start
    mov esi, 0
    mov edx, (kernel_va_end - kernel_va_start) / pg_size
    call .map_pages

    mov edi, ioapic_va_start
    mov esi, ioapic_pa_start
    mov edx, (ioapic_va_end - ioapic_va_start) / pg_size
    call .map_pages

    mov edi, lapic_va_start
    mov esi, lapic_pa_start
    mov edx, (lapic_va_end - lapic_va_start) / pg_size
    call .map_pages

    mov edi, kpgdir_va
    mov esi, kpgdir_pa
    mov edx, 1
    call .map_pages
   

    ; page table setup is complete

    mov eax, 10100000b                ; Set the PAE and PGE bit.
    mov cr4, eax
 
    mov edx, kpgdir_pa                      ; Point CR3 at the PML4.
    mov cr3, edx
 
    mov ecx, 0xC0000080               ; Read from the EFER MSR. 
    rdmsr    
    or eax, 0x00000100                ; Set the LME bit.
    wrmsr
 
    mov ebx, cr0                      ; Activate long mode -
    or ebx,0x80000001                 ; - by enabling paging and protection simultaneously.
    mov cr0, ebx 

    jmp dword 0x0008:long_mode

    .alloc_one_clear_page: ; void alloc_one_clear_page(edi:specified_pa = 0);
        cld
        push edi
        push ecx
        mov edi, [next_physical_page_addr] ; no specified pa, use global allocator
        mov ecx, pg_size / dword_size
        xor eax, eax
        rep stosd
        ; .memset:
        ;     mov dword [edi], 0
        ;     add edi, 4
        ;     loop .memset
        mov eax, edi
        add edi, pg_size
        mov [next_physical_page_addr], edi
        pop ecx
        pop edi
        ret

    .map_pages: ; void map_pages(edi:va_start, esi:pa_start=0, dx:pages); edx makes the total size > 512 :)
        mov eax, kpgdir_pa ; root kernel page table directory
        push ecx ; pages
        .map_one_page:
            mov ecx, pg_table_level ; 4
            .map_one_level:
                ; pte_addr = edi + (edi >> ((ecx - 1) * 9 + 12)) & 0x1FF) * 8
                push edi ; target va address
                push eax ; current page table directory
                dec ecx
                mov eax, ecx
                push ebx
                mov ebx, pg_entry_index_bits
                mul ebx
                pop ebx
                add eax, pg_entry_offset_bits
                push ecx
                mov ecx, eax
                shr edi, cl
                pop ecx
                inc ecx
                and edi, pg_entry_index_mask
                pop eax
                lea edi, [eax + edi * pg_entry_size] ; page entry address
                mov ebx, [edi] ; page entry content
                cmp ebx, 0
                jne .current_level_complete
                cmp ecx, 1
                jne .not_the_last_level
                cmp esi, 0
                je .identity_map
                push eax
                call .alloc_one_clear_page
                or eax, pg_perm_bits
                mov [edi], eax
                pop eax
                jmp .current_level_complete
                .identity_map:
                    mov ebx, esi
                    or ebx, pg_perm_bits
                    mov [edi], ebx
                jmp .current_level_complete
                .not_the_last_level:
                    push eax
                    call .alloc_one_clear_page
                    or eax, pg_perm_bits
                    mov [edi], eax
                    pop eax
                .current_level_complete:
                    ; nop
                pop edi
                dec ecx
                jnz .map_one_level
            add edi, pg_size
            cmp esi, 0
            je .no_need_to_inc_pa
            add esi, pg_size
            .no_need_to_inc_pa:
                ; nop
            pop ecx
            dec ecx
            jnz .map_one_page
        ret

reachable:
    mov al, 0x30
    mov ah, 0xE
    int 0x10
    jmp $

next_physical_page_addr: dd 0x100000

gdt: dq 0
     dq 0x00209A0000000000 ; code L, P, S, E, RW
     dq 0x0000920000000000 ; data P, S, RW

gdt_desc:
    .gdt_size: dw gdt_desc - gdt - 1
    .gdt_base: dd gdt
;-------------------------
BITS 64
;-------------------------
long_mode:
    mov ax, 0x0010
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, kernel_va_end
    call load_kernel ; never returns

    .error:
        hlt
        jmp .error





