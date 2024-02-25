#pragma once
#include "types.h"
// === pic.c ========
void picinit();

// === lapic.c ======
void lapicinit();
void lapiceoi();

// === ioapic.c ====
void ioapicinit();
void ioapicenable(int irq, int cpunum);

// === uart.c ======
void uartinit();
void uartputc(char c);

// === traps.c =====
void trapinit();
void idtinit();

// === print.c =====
void prints(char* str);
void printd(uint64_t i);
void printx(uint64_t i);