#include "x86.h"
#include "proc.h"
#include "defs.h"
#include "macros.h"

#define MAXPROCS 16

struct cpu cpus[2];

struct {
    struct proc procs[MAXPROCS];
} ptable;

int nextpid = 1;

extern void trapret();

static struct proc* allocproc() {
    struct proc* proc = NULL;
    for (int i = 0; i < MAXPROCS; ++i) {
        if (ptable.procs[i].state == UNUSED) {
            proc = &ptable.procs[i];
            break;
        }
    }
    if (proc == NULL)
        return NULL;
    proc->state = EMBRYO;
    proc->pid = nextpid++;

    proc->kstack = kalloc();
    proc->pgdir = setupuvm();

    void* rsp = proc->kstack + PG_SIZE;
    rsp -= sizeof(struct trapframe);
    proc->tf = rsp;
    rsp -= 8;
    *(uint64_t*)rsp = trapret;
    rsp -= sizeof(struct context);
    proc->cxt = rsp;
    proc->cxt->rip = trapret;

    return proc;
    
    
}

void userinit() {
    struct proc* proc = allocproc();
}

void scheduler() {
    sti();
    while (1) {
        for (struct proc* p = ptable.procs; p < &ptable.procs[MAXPROCS]; ++p) {
            if (p->state == RUNNABLE) {
                lcr3(KV2P(p->pgdir));
            }
        }
    }
}