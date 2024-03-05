#include "syscall.h"
#include "types.h"
#include "defs.h"
#include "proc.h"
#include "traps.h"

static uint64_t get_arg(uint8_t index) {
    switch (index) {
        case 0:
            return mycpu()->p->tf->rdi;
        case 1:
            return mycpu()->p->tf->rsi;
        case 2:
            return mycpu()->p->tf->rdx;
        case 3:
            return mycpu()->p->tf->rcx;
        case 4:
            return mycpu()->p->tf->r8;
        case 5:
            return mycpu()->p->tf->r9;
        default:
            panic("syscall only supports 6 arguments");
    }
}


void sys_yield() {
    mycpu()->p->state = RUNNABLE;
    swtch(&mycpu()->p->cxt, mycpu()->cxt);
}

void sys_echo() {
    prints(get_arg(0));
}

void sys_add() {
    mycpu()->p->tf->rax = get_arg(0) + get_arg(1);
}

void (*syscall_map[])() = {
    [SYS_yield] = sys_yield,
    [SYS_echo] = sys_echo,
    [SYS_add] = sys_add,
};

void syscall() {
    syscall_map[mycpu()->p->tf->rax]();
}