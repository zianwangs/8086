BITS 64
extern trapret
global swtch

swtch:
  push r15
  push r14
  push r13
  push r12
  push rbp
  push rbx

  ; Switch stacks
  mov [rdi], rsp
  mov rsp, rsi

  pop rbx
  pop rbp
  pop r12
  pop r13
  pop r14
  pop r15

  ret
