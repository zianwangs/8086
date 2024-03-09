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

void trapret();

struct cpu* mycpu() {
    return &cpus[lapicid()];
}

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
    // consec fall through ret: forkret -> trapret
    // no need for forkret for now
    // rsp -= 8;
    // *(uint64_t*)rsp = trapret;
    rsp -= sizeof(struct context);
    proc->cxt = rsp;
    proc->cxt->rip = trapret;

    return proc;
    
    
}
// static void user2init() {
//     extern char _binary_init2_bin_start[], _binary_init2_bin_end[];
//     struct proc* proc = allocproc();
//     void* uaddr = uva2kva(proc->pgdir, 0);
//     uint64_t size = _binary_init2_bin_end - _binary_init2_bin_start;
//     memcpy(uaddr, _binary_init2_bin_start, size);

//     proc->tf->cs =  (3 << 3) | 3;
//     proc->tf->rflags = 0x200;
//     proc->tf->rsp = (UPROG_PAGES + USTACK_PAGES) * PG_SIZE;
//     proc->tf->rip = 0;
//     proc->tf->ss = (4 << 3) | 3;

//     proc->state = RUNNABLE;
// }
void userinit() {
    extern char _binary_init_bin_start[], _binary_init_bin_end[];
    struct proc* proc = allocproc();
    void* uaddr = uva2kva(proc->pgdir, 0);
    uint64_t size = _binary_init_bin_end - _binary_init_bin_start;
    memcpy(uaddr, _binary_init_bin_start, size);

    proc->tf->cs =  (3 << 3) | 3;
    proc->tf->rflags = 0x200;
    proc->tf->rsp = (UPROG_PAGES + USTACK_PAGES) * PG_SIZE;
    proc->tf->rip = 0;
    proc->tf->ss = (4 << 3) | 3;
    proc->state = RUNNABLE;
}

void scheduler() {
    sti();
    struct cpu* c = mycpu();
    while (1) {
        for (struct proc* p = ptable.procs; p < &ptable.procs[MAXPROCS]; ++p) {
            if (p->state == RUNNABLE) {
                c->p = p;
                lcr3(KV2P(p->pgdir));
                
                // c->gdt[5].type = 0x9;
                // no need to ltr again, tested
                c->ts.rsp0_lo = p->kstack + PG_SIZE;
                // c->ts.iomb = 0xFFFF;
        
                // ltr(5 << 3);
                // printx(trapret);
                p->state = RUNNING;
       
                swtch(&c->cxt, p->cxt);
                lcr3(KPGDIR);
                c->p = NULL;
            
            }
        }
    }
}
