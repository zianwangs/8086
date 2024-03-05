#pragma once
#include "types.h"
// === pic.c ========
void picinit();

// === lapic.c ======
void lapicinit();
void lapiceoi();
uint8_t lapicid();

// === ioapic.c ====
void ioapicinit();
void ioapicenable(int irq, int cpunum);

// === uart.c ======
void uartinit();
void uartputc(uint8_t c);
uint8_t uartgetc();

// === traps.c =====
void trapinit();
void idtinit();

// === proc.c =======
struct cpu* mycpu();
void userinit();
void scheduler();

// === swtch.asm ====
void swtch(struct context** out, struct context* in);

// === console.c ====
void consoleinit();
void consoleintr();

// === kalloc.c =====
void kinit(void* start, void* end);
void* kalloc();
void kfree(void* addr);

// === vm.c =========
void seginit();
void* setupuvm();
void* uva2kva(uint64_t* pgdir, void* uva);

// === print.c =====
void prints(char* str);
void printd(uint64_t i);
void printx(uint64_t i);

// === syscall.c ===
void sys_yield();
void syscall();

// === util.c ======
void memcpy(char* dest, char* src, uint64_t count);
void panic(char* msg);