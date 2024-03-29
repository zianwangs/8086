#include "types.h"
#include "traps.h"

#define ID         (0x0020 / 4)   // ID
#define VER        (0x0030 / 4)   // Version
#define TPR        (0x0080 / 4)   // Task Priority
#define EOI        (0x00B0 / 4)   // EOI
#define SVR        (0x00F0 / 4)   // Spurious Interrupt Vector
#define ENABLE     0x00000100   // Unit Enable
#define ESR        (0x0280 / 4)   // Error Status
#define ICRLO      (0x0300 / 4)   // Interrupt Command
#define INIT       0x00000500   // INIT/RESET
#define STARTUP    0x00000600   // Startup IPI
#define DELIVS     0x00001000   // Delivery status
#define ASSERT     0x00004000   // Assert interrupt (vs deassert)
#define DEASSERT   0x00000000
#define LEVEL      0x00008000   // Level triggered
#define BCAST      0x00080000   // Send to all APICs, including self.
#define BUSY       0x00001000
#define FIXED      0x00000000
#define ICRHI      (0x0310 / 4)   // Interrupt Command [63:32]
#define TIMER      (0x0320 / 4)   // Local Vector Table 0 (TIMER)
#define X1         0x0000000B   // divide counts by 1
#define X2         0x00000000
#define X4         0x00000001
#define X8         0x00000002
#define X16        0x00000003
#define X128       0x0000000A
#define ONESHOT    0x00000000
#define PERIODIC   0x00020000   // Periodic
#define DEADLINE   0x00040000
#define PCINT      (0x0340 / 4)   // Performance Counter LVT
#define LINT0      (0x0350 / 4)   // Local Vector Table 1 (LINT0)
#define LINT1      (0x0360 / 4)   // Local Vector Table 2 (LINT1)
#define ERROR      (0x0370 / 4)   // Local Vector Table 3 (ERROR)
#define MASKED     0x00010000   // Interrupt masked
#define TICR       (0x0380 / 4)   // Timer Initial Count
#define TCCR       (0x0390 / 4)   // Timer Current Count
#define TDCR       (0x03E0 / 4)   // Timer Divide Configuration

volatile uint32_t* lapic = 0xFEE00000;

static void lapicw(int index, uint32_t value) {;
  lapic[index] = value;
  lapic[ID];
}


void lapicinit() {

  lapicw(SVR, ENABLE | (T_IRQ0 + IRQ_SPURIOUS));
  // The timer repeatedly counts down at bus frequency
  // from lapic[TICR] and then issues an interrupt.
  // If xv6 cared more about precise timekeeping,
  // TICR would be calibrated using an external time source.
  // lapicw(TDCR, X2);
  lapicw(TDCR, X128);
  lapicw(TIMER, PERIODIC | (T_IRQ0 + IRQ_TIMER));
  // TODO: need to know the actual bus freq using cpuid
  // coarsely calibrated to be 1GHz, so now it's a 1s
  // time slice, might need to be shorter
  lapicw(TICR, 1000000000 / 128);

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

uint8_t lapicid() {
  return lapic[ID] >> 24;
}

// Acknowledge interrupt.
void lapiceoi()
{
    lapicw(EOI, 0);
}


