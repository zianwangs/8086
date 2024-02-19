BITS 16
;-------------------------
global entry
; extern load_kernel
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
pg_table_levels      equ 0x4
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
    ; cli
    lgdt [gdt_desc]
    
    in al, 0x92
    or al, 0000_0010B
    out 0x92, al
    
    mov esp, boot_sector_pa_end
    
    call .alloc_one_clear_page ; for kpgdir
   
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
        mov ebp, [next_physical_page_addr] ; no specified pa, use global allocator
        lea eax, [ebp + pg_size]
        mov [next_physical_page_addr], eax
        xor eax, eax
        .memset:
            mov dword [ebp + eax * 4], 0
            inc eax
            cmp eax, pg_size / pg_entry_size
            jne .memset
        mov eax, ebp
        ret

    .map_pages:
        mov ebp, kpgdir_pa ; no other registers :)
        .map_one_page:
            dec edx
            cmp edx, 0xFFFFFFFF
            je .end
            mov ebx, pg_table_levels
            .map_one_level:
            ; r9d=ebx, r8d=eax, rax=ebp
                lea ecx, [ebx * 9]
                mov eax, edi
                add ecx, 3
                shr eax, cl
                mov ecx, eax
                shl ecx, 3
                and ecx, 0xFF8
                add ecx, ebp
                cmp ebx, 0x1
                jne .not_the_last_level
                test esi, esi
                je .not_the_last_level
                mov [ecx], esi
                jmp .exists
                .not_the_last_level:
                    cmp dword [ecx], 0
                    jne .exists
                call .alloc_one_clear_page
                mov [ecx], eax
                .exists:
                    mov ebp, [ecx]
                dec ebx
                jne .map_one_level
                jmp .map_one_page
                .end:
                    ret


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
    ; call load_kernel ; never returns
    jmp 0x80000000

    .error:
        hlt
        jmp .error





