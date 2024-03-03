#pragma once
#include "types.h"

// note AT&T syntax uses mov src, dest

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile("out %0, %1" : : "a" (data), "d" (port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile("in %1, %0" : "=a" (data) : "d" (port));
    return data;
}

static inline void cli() {
    asm volatile("cli");
}

static inline void sti() {
    asm volatile("sti");
}

static inline void lidt(void* addr, uint16_t size) {
    uint16_t idt[5];  // volatile ?
    idt[0] = size - 1;
    *(uint64_t*)(idt + 1) = (uint64_t)addr;
    asm volatile("lidt (%0)" : : "r" (idt));
}

static inline void lgdt(void* addr, uint16_t size) {
    uint16_t gdt[5];
    gdt[0] = size - 1;
    *(uint64_t*)(gdt + 1) = (uint64_t)addr;
    asm volatile("lgdt (%0)" : : "r" (gdt));
}

static inline void ltr(uint16_t seg_selector) {
    asm volatile("ltr %0" : : "r" (seg_selector));
}

static inline void lcr3(uint64_t pgdir) {
    asm volatile("mov %0, %%cr3" : : "r" (pgdir));
}