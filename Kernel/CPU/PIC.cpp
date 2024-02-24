#include <Kernel/CPU/IDT.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/IO.h>
#include <Kernel/Logger.h>

#define PIC1_COMMAND 0x0020
#define PIC1_DATA 0x0021
#define PIC2_COMMAND 0x00A0
#define PIC2_DATA 0x00A1
#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28
#define PIC_EOI 0x20

namespace PIC {

void mask(uint32_t mask)
{
    uint8_t val;
    uint16_t port;

    if (mask < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        mask -= 8;
    }

    val = IO::inb(port) | (1 << mask);
    IO::outb(port, val);
}

void unmask(uint32_t mask)
{
    uint8_t val;
    uint16_t port;

    if (mask < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        mask -= 8;
    }

    val = IO::inb(port) & ~(1 << mask);
    IO::outb(port, val);
}

void eoi(uint8_t irq)
{
    if (irq >= 8) {
        IO::outb(PIC2_COMMAND, PIC_EOI);
    }

    IO::outb(PIC1_COMMAND, PIC_EOI);
}

void init()
{
    IO::outb(PIC1_COMMAND, 0x11);
    IO::outb(PIC2_COMMAND, 0x11);
    IO::outb(PIC1_DATA, PIC1_OFFSET);
    IO::outb(PIC2_DATA, PIC2_OFFSET);
    IO::outb(PIC1_DATA, 0x04);
    IO::outb(PIC2_DATA, 0x02);
    IO::outb(PIC1_DATA, 0x01);
    IO::outb(PIC2_DATA, 0x01);

    // Mask all IRQs
    IO::outb(PIC1_DATA, 0xFB);
    IO::outb(PIC2_DATA, 0xFF);

    dbgprintf("PIC", "Initialized PIC: offset1=0x%x offset2=0x%x\n", PIC1_OFFSET, PIC2_OFFSET);

    sti();
}

void disable()
{
    IO::outb(PIC1_DATA, 0xff);
    IO::outb(PIC2_DATA, 0xff);
}

}
