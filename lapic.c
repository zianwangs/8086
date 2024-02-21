#include "types.h"
#include "traps.h"

#define ID         (0x0020 / 8)   // ID
#define VER        (0x0030 / 8)   // Version
#define TPR        (0x0080 / 8)   // Task Priority
#define EOI        (0x00B0 / 8)   // EOI
#define SVR        (0x00F0 / 8)   // Spurious Interrupt Vector
#define ENABLE     0x00000100   // Unit Enable
#define ESR        (0x0280 / 8)   // Error Status
#define ICRLO      (0x0300 / 8)   // Interrupt Command
#define INIT       0x00000500   // INIT/RESET
#define STARTUP    0x00000600   // Startup IPI
#define DELIVS     0x00001000   // Delivery status
#define ASSERT     0x00004000   // Assert interrupt (vs deassert)
#define DEASSERT   0x00000000
#define LEVEL      0x00008000   // Level triggered
#define BCAST      0x00080000   // Send to all APICs, including self.
#define BUSY       0x00001000
#define FIXED      0x00000000
#define ICRHI      (0x0310 / 8)   // Interrupt Command [63:32]
#define TIMER      (0x0320 / 8)   // Local Vector Table 0 (TIMER)
#define X1         0x0000000B   // divide counts by 1
#define PERIODIC   0x00020000   // Periodic
#define PCINT      (0x0340 / 8)   // Performance Counter LVT
#define LINT0      (0x0350 / 8)   // Local Vector Table 1 (LINT0)
#define LINT1      (0x0360 / 8)   // Local Vector Table 2 (LINT1)
#define ERROR      (0x0380 / 8)   // Local Vector Table 3 (ERROR)
#define MASKED     0x00010000   // Interrupt masked
#define TICR       (0x0380 / 8)   // Timer Initial Count
#define TCCR       (0x0390 / 8)   // Timer Current Count
#define TDCR       (0x03E0 / 8)   // Timer Divide Configuration

volatile uint64_t* lapic = 0xFEE00000;

static void lapicw(int index, uint64_t value) {
  lapic[index] = value;
  lapic[ID];
}

void lapicinit() {

  lapicw(SVR, ENABLE | (T_IRQ0 + IRQ_SPURIOUS));
  // The timer repeatedly counts down at bus frequency
  // from lapic[TICR] and then issues an interrupt.
  // If xv6 cared more about precise timekeeping,
  // TICR would be calibrated using an external time source.
  lapicw(TDCR, X1);
  lapicw(TIMER, PERIODIC | (T_IRQ0 + IRQ_TIMER));
  lapicw(TICR, 10000000);

  // Disable logical interrupt lines.
  lapicw(LINT0, MASKED);
  lapicw(LINT1, MASKED);

  // Disable performance counter overflow interrupts
  // on machines that provide that interrupt entry.
  if(((lapic[VER]>>16) & 0xFF) >= 4)
    lapicw(PCINT, MASKED);

  // Map error interrupt to IRQ_ERROR.
  lapicw(ERROR, T_IRQ0 + IRQ_ERROR);

  // Clear error status register (requires back-to-back writes).
  lapicw(ESR, 0);
  lapicw(ESR, 0);

  // Ack any outstanding interrupts.
  lapicw(EOI, 0);

  // Send an Init Level De-Assert to synchronise arbitration ID's.
  lapicw(ICRHI, 0);
  lapicw(ICRLO, BCAST | INIT | LEVEL);
  while(lapic[ICRLO] & DELIVS)
    ;

  // Enable interrupts on the APIC (but not on the processor).
  lapicw(TPR, 0);
}

int lapicid() {
  return lapic[ID] >> 24;
}

// Acknowledge interrupt.
void lapiceoi()
{
    lapicw(EOI, 0);
}


