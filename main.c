#include "defs.h"

void main() {
    picinit();
    lapicinit();
    ioapicinit();
    uartinit();
    trapinit();
    while (1);
}
