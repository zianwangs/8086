#include "syscall.h"

#define SYSCALL(name) \
  .globl name; \
  name: \
    mov $SYS_ ## name, %rax; \
    int $0x80; \
    ret

SYSCALL(yield)
SYSCALL(echo)
SYSCALL(add)
