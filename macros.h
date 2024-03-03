#pragma once

#define KERNEL_PA_START 0x200000UL
#define KERNEL_VA_START 0x80000000UL
#define KERNEL_VA_END   0x80100000UL
#define IOAPIC_START    0xFEC00000UL
#define LAPIC_START     0xFEE00000UL  

#define KV2P(addr)      ((addr) - KERNEL_VA_START + KERNEL_PA_START)
#define KP2V(addr)      ((addr) - KERNEL_PA_START + KERNEL_VA_START)
#define KPGDIR          0x100000UL

#define PG_SIZE 4096UL
#define UPROG_PAGES 1
#define USTACK_PAGES 1

#define PGROUNDUP(addr) ((((addr) + (PG_SIZE - 1)) & ~(PG_SIZE - 1))) 
#define PGROUNDDOWN(addr) ((addr) & ~(PG_SIZE - 1))
