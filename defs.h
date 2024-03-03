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
void userinit();
void scheduler();

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

// === print.c =====
void prints(char* str);
void printd(uint64_t i);
void printx(uint64_t i);