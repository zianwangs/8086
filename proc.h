#pragma once
#include "types.h"
#include "traps.h"

struct segdesc {
  uint16_t seg_limit : 16;  // Low bits of segment limit
  uint16_t base_15_0 : 16; // Low bits of segment base address
  uint16_t base_23_16 : 8; // Middle bits of segment base address
  uint8_t type : 4;       // Segment type (see STS_ constants)
  uint8_t s : 1;          // 0 = system, 1 = application
  uint8_t dpl : 2;        // Descriptor Privilege Level
  uint8_t p : 1;          // Present
  uint8_t lim_19_16 : 4;  // High bits of segment limit
  uint8_t avl : 1;        // Unused (available for software use)
  uint8_t l : 1;       // long mode code segment
  uint8_t db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
  uint8_t g : 1;          // Granularity: limit scaled by 4K when set
  uint8_t base_31_24 : 8; // High bits of segment base address
};

struct taskstate {
    uint32_t rsv1;
    uint32_t rsp0_lo;
    uint32_t rsp0_hi;
    uint32_t rsp1_lo;
    uint32_t rsp1_hi;
    uint32_t rsp2_lo;
    uint32_t rsp2_hi;
    uint32_t rsv2;
    uint32_t rsv3;
    uint32_t lst1_lo;
    uint32_t lst1_hi;
    uint32_t lst2_lo;
    uint32_t lst2_hi;
    uint32_t lst3_lo;
    uint32_t lst3_hi;
    uint32_t lst4_lo;
    uint32_t lst4_hi;
    uint32_t lst5_lo;
    uint32_t lst5_hi;
    uint32_t lst6_lo;
    uint32_t lst6_hi;
    uint32_t lst7_lo;
    uint32_t lst7_hi;
    uint32_t rsv4;
    uint32_t rsv5;
    uint16_t rsv6;
    uint16_t iomb;
};

struct context {
    uint64_t rbx;
    uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;
};

struct cpu {
    uint8_t cpuid;
    struct segdesc gdt[7];
    struct taskstate ts;
    struct context* cxt;
    struct proc* p;
};


enum procstate {
    UNUSED, EMBRYO, RUNNABLE, RUNNING, SLEEPING, ZOMBIE
};

struct proc {
    int pid;
    enum procstate state;
    void* pgdir;
    uint64_t size;
    void* kstack;
    struct context* cxt;
    struct trapframe* tf;
    
};