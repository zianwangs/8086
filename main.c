#include "defs.h"

void main() {
    picinit();
    lapicinit();
    ioapicinit();
    uartinit();
    trapinit();
    idtinit();
    while (1);
}
