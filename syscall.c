#include "syscall.h"
#include "types.h"
#include "defs.h"
#include "proc.h"
#include "traps.h"

static uint64_t get_arg_1() {
    return mycpu()->p->tf->rdi;
}

static uint64_t get_arg_2() {
    return mycpu()->p->tf->rsi;
}

void sys_yield() {
    mycpu()->p->state = RUNNABLE;
    swtch(&mycpu()->p->cxt, mycpu()->cxt);
}

void sys_echo() {
    prints(get_arg_1());
}
void (*syscall_map[])() = {
    [SYS_yield] = sys_yield,
    [SYS_echo] = sys_echo,
};

void syscall() {
    syscall_map[mycpu()->p->tf->rax]();
}