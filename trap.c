
#include "types.h"

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

extern char end[];

struct gatedesc idt[256];

void trapinit() {
  idt[255].cs = 8;
  void* p = &end;
  // for(int i = 0; i < 256; i++)
  //   SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  // SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);


}