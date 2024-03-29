// Intel 8250 serial port (UART).
#include "x86.h"
#include "defs.h"
#include "traps.h"

#define COM1    0x3f8

static int uart = 0;    // is there a uart?

void uartinit()
{

  // Turn off the FIFO
  outb(COM1 + 2, 0);

  // 9600 baud, 8 data bits, 1 stop bit, parity off.
  outb(COM1 + 3, 0x80);    // Unlock divisor
  outb(COM1 + 0, 115200 / 9600);
  outb(COM1 + 1, 0);
  outb(COM1 + 3, 0x03);    // Lock divisor, 8 data bits.
  outb(COM1 + 4, 0);
  outb(COM1 + 1, 0x01);    // Enable receive interrupts.

  // If status is 0xFF, no serial port.
  if(inb(COM1 + 5) == 0xFF)
    return;

  uart = 1;

  // Acknowledge pre-existing interrupt conditions;
  // enable interrupts.
  inb(COM1 + 2);
  inb(COM1 + 0);
  ioapicenable(IRQ_COM1, 0);

//   Announce that we're here.
  for(char* p = "\n8086 booting...\n"; *p; p++)
    uartputc(*p);

}

// Spin for a given number of microseconds.
// On real hardware would want to tune this dynamically.
static void microdelay(int us) {
  
}

void uartputc(uint8_t c)
{
  if(!uart)
    return;
  for(int i = 0; i < 128 && !(inb(COM1 + 5) & 0x20); i++)
    microdelay(10);
  outb(COM1 + 0, c);
}

uint8_t uartgetc(void)
{
  if(!uart)
    return -1;
  if(!(inb(COM1 + 5) & 0x01))
    return -1;
  return inb(COM1 + 0);
}