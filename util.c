#include "types.h"
#include "defs.h"

void memcpy(char* dest, char* src, uint64_t count) {
    while (count--)
        *dest++ = *src++;
}

void panic(char* msg) {
    prints(msg);
    while (1);
}