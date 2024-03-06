#include "defs.h"
#include "macros.h"

extern char end[];

void main() {
    kinit(end, KERNEL_VA_END - PG_SIZE);  // 1MB, minus 1 page for stack
    seginit();
    picinit();
    lapicinit();
    ioapicinit();
    uartinit();
    trapinit();
    consoleinit();
    userinit();
    // userinit();
    
    idtinit(); // load idt and sti, should be the last step
    
    scheduler(); // now it's just an infinite loop;
    // never returns
}
