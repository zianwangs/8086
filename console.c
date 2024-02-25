#include "defs.h"
#include "traps.h"
#include "x86.h"

#define BS  0x08
#define LF  0x0A
#define CR  0x0D
#define SP  0x20
#define DEL 0x7F
#define ERR 0xFF


static char buf[128];
static uint8_t buf_len = 0;

static void flush() {
    buf[buf_len] = '\0';
    uartputc(LF);
    if (buf_len)
        prints(buf);
    buf_len = 0;
    uartputc('$');
    uartputc(SP);
}

static void backspace() {
    if (buf_len == 0)
        return;
    uartputc(BS);
    uartputc(SP);
    uartputc(BS);
    buf[--buf_len] = '\0';
}
 
void consoleintr() {
    uint8_t c = uartgetc();
    switch (c)
    {
        case LF:
        case CR:
            flush();
            break;
        case BS:
        case DEL:
            backspace();
            break;
        case ERR:
            break;
        default:
            buf[buf_len++] = c;
            uartputc(c);
            break;
    }
    
}
void consoleinit() {
    uartputc('$');
    uartputc(SP);
    // ioapicenable(IRQ_KBD, 0);
}