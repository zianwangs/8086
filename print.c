#include "defs.h"

void prints(char* str) {
    for (; *str; ++str)
        uartputc(*str);
    uartputc('\n');
}

#define BUF_LEN 65

void printd(uint64_t i) {
    char buf[BUF_LEN] = {0};
    int index = BUF_LEN - 1;
    do {
        buf[--index] = (i % 10) + '0';
        i /= 10;
    } while (i);
    prints(buf + index);
}

void printx(uint64_t i) {
    char buf[BUF_LEN] = {0};
    int index = BUF_LEN - 2;
    do {
        uint8_t r = i % 16;
        buf[--index] = r < 10 ? r + '0' : r - 10 + 'A';
        i /= 16;
    } while (i);
    buf[--index] = 'x';
    buf[--index] = '0';
    prints(buf + index);
}