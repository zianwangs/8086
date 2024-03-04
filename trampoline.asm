BITS 64

extern trap
global trampoline
global trapret

; SECTION .text
trampoline:
    ; ignroe segment registers for now 
    ; push ds  ; need to confirm rsp is auto algined by 64 bits
    ; push es
    ; push gs
    ; push fs
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    ; mov ax, 0x10
    ; mov ds, ax
    ; mov es, ax

    mov rdi, rsp
    call trap
    

trapret:

  pop r11
  pop r10
  pop r9
  pop r8
  pop rdi
  pop rsi
  pop rbp
  pop rbx
  pop rdx
  pop rcx
  pop rax

  add rsp, 16 ; for trapno and errno
  iretq

   
