#pragma once
#include "types.h"

// note AT&T syntax uses mov src, dest

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile("out %0, %1" : : "a"(data), "d"(port));
}

static inline void cli() {
    asm volatile("cli");
}

static inline void sti() {
    asm volatile("sti");
}