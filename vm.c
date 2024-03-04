#include "defs.h"
#include "proc.h"
#include "x86.h"
#include "macros.h"

#define PTE_P 0x1
#define PTE_W 0x2
#define PTE_U 0x4

extern struct cpu cpus[];

void seginit() {
    uint8_t cpuid = lapicid();
    struct cpu* c = &cpus[cpuid];
    c->cpuid = cpuid;
    ((uint64_t*)(c->gdt))[1] = 0x00209A0000000000UL;
    ((uint64_t*)(c->gdt))[2] = 0x0000920000000000UL;
    ((uint64_t*)(c->gdt))[3] = 0x0020FA0000000000UL;
    ((uint64_t*)(c->gdt))[4] = 0x0000F20000000000UL;
    uint64_t tss_addr = &c->ts;
    uint64_t tss_limit = sizeof(c->ts) - 1;
    uint64_t seg_limit_15_0 = tss_limit & 0xFFFF;
    uint64_t base_15_0 = tss_addr & 0xFFFF;
    uint64_t base_23_16 = (tss_addr >> 16) & 0xFF;
    uint64_t seg_limit_19_16 = (tss_limit >> 16) & 0xF;
    uint64_t base_31_24 = (tss_addr >> 24) & 0xFF;
    uint64_t base_63_32 = (tss_addr >> 32) & 0xFFFFFFFF;
    ((uint64_t*)(c->gdt))[5] = (base_31_24 << 56) | (0x0UL << 52) | (seg_limit_19_16 << 48) | (0x8UL << 44) | (0x9UL << 40) | (base_23_16 << 32) | (base_15_0 << 16) | seg_limit_15_0;
    ((uint64_t*)(c->gdt))[6] = base_63_32;
    lgdt(c->gdt, sizeof(c->gdt));
    // c->ts.rsp0_lo = 0x80010000;
    // c->ts.iomb = 0xFFFF;
    // ltr(5 << 3);
}

static int mappages(uint64_t* pgdir, void* va, void* pa, uint16_t pages, uint16_t perm) {
    while (pages--) {
        uint64_t* cur_pgdir = pgdir;
        for (int i = 3; i >= 0; --i) {
            int index = (uint64_t)va >> (i * 9 + 12) & 0x1FF;
            if (i == 0) {
                if (cur_pgdir[index] != 0)
                    return -1;
                cur_pgdir[index] = (uint64_t)pa | perm;            
            } else {
                if (cur_pgdir[index] == 0)
                    cur_pgdir[index] = (uint64_t)KV2P(kalloc()) | perm;
                cur_pgdir = KP2V(cur_pgdir[index] & ~(PG_SIZE - 1)); 
            }
        }
        va += PG_SIZE;
        pa += PG_SIZE;
    }
    return 0;
}

// won't work for KPGDIR, because it's below KERNEL_PA_START
void* uva2kva(uint64_t* pgdir, void* uva) {
    for (int i = 3; i >= 0; --i) {
        int index = (uint64_t)uva >> (i * 9 + 12) & 0x1FF;
        if (pgdir[index] == 0)
            return 0;
        pgdir = KP2V(pgdir[index] & ~(PG_SIZE - 1));
       
    }
    return (uint64_t)pgdir | ((uint64_t)uva & (PG_SIZE - 1));
}

// alloc 2 pages, return the root pgdir
void* setupuvm() {
    void* pgdir = kalloc();
    uint64_t va = 0;
    // user space
    for (int i = 0; i < UPROG_PAGES; va += PG_SIZE, ++i)
        mappages(pgdir, va, KV2P(kalloc()), 1, PTE_U | PTE_W | PTE_P);
    for (int i = 0; i < USTACK_PAGES; va += PG_SIZE, ++i)
        mappages(pgdir, va, KV2P(kalloc()), 1, PTE_U | PTE_W | PTE_P);
    // kernel space
    mappages(pgdir, KERNEL_VA_START, KERNEL_PA_START, (KERNEL_VA_END - KERNEL_VA_START) / PG_SIZE, PTE_W | PTE_P);
    mappages(pgdir, IOAPIC_START, IOAPIC_START, 1, PTE_W | PTE_P);
    mappages(pgdir, LAPIC_START, LAPIC_START, 1, PTE_W | PTE_P);
    return pgdir;
}
