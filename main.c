#include "defs.h"

void main() {
    picinit();
    lapicinit();
    ioapicinit();
    uartinit();
    trapinit();
    consoleinit();

    idtinit(); // load idt and sti, should be the last step
    scheduler(); // now it's just an infinite loop;
    // never returns
}
