#include "x86.h"

#define PIC_PORT_MASTER 0x20
#define PIC_PORT_SLAVE  0xA0

void picinit() {
    outb(PIC_PORT_MASTER + 1, 0xFF);
    outb(PIC_PORT_SLAVE + 1, 0xFF);
}
