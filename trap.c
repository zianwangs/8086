
#include "types.h"
#include "traps.h"
#include "x86.h"
#include "defs.h"

struct gatedesc {
  uint16_t offset_15_0;   // low 16 bits of offset in segment
  uint16_t cs;         // code segment selector
  uint8_t ist : 5;        // # args, 0 for interrupt/trap gates
  uint8_t rsv1 : 3;        // reserved(should be zero I guess)
  uint8_t type : 4;        // type(STS_{IG32,TG32})
  uint8_t s : 1;           // must be 0 (system)
  uint8_t dpl : 2;         // descriptor(meaning new) privilege level
  uint8_t p : 1;           // Present
  uint16_t offset_31_16;  // high bits of offset in segment
  uint32_t offset_63_32;
  uint32_t rsv;
};

struct trapframe {
  // registers as pushed by pusha
  // xmm registers?
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rbp;
  uint64_t rbx;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rax;

  // rest of trap frame, ignore segment registers for now
  // uint64_t gs;
  // uint64_t fs;
  // uint64_t es;
  // uint64_t ds;
  uint64_t trapno;

  // below here defined by x86 hardware
  uint64_t errno;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;

  // below here only when crossing rings, such as from user to kernel
  uint64_t rsp;
  uint64_t ss;
};

extern char end[];
extern uint64_t vector[];
struct gatedesc idt[256];

#define SETGATE(gate, istrap, sel, offset, d)       \
{                                                   \
  (gate).offset_15_0 = (offset) & 0xffff;           \
  (gate).cs = (sel);                                \
  (gate).ist = 0;                                   \
  (gate).rsv1 = 0;                                  \
  (gate).type = (istrap) ? 0xF : 0xE;               \
  (gate).s = 0;                                     \
  (gate).dpl = (d);                                 \
  (gate).p = 1;                                     \
  (gate).offset_31_16 = (offset) >> 16;             \
  (gate).offset_63_32 = (offset) >> 32;             \
}

void trapinit() {
  for(int i = 0; i < 256; i++)
    SETGATE(idt[i], 0, 0x8, vector[i], 0);
  SETGATE(idt[T_SYSCALL], 1, 0x8, vector[T_SYSCALL], 3);
}

void idtinit() {
  lidt(idt, sizeof(idt));
  sti();
}

uint32_t tick = 0;
void trap(struct trapframe* tf) {

  if (tf->trapno == IRQ_TIMER + T_IRQ0) {
    printd(tick++);
    lapiceoi();
  }
  // lapiceoi();
}