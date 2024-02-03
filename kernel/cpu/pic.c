#include <cpu/idt.h>
#include <cpu/pic.h>
#include <logger.h>
#include <sys/io.h>

void pic_mask(uint32_t mask)
{
    uint8_t val;
    uint16_t port;

    if (mask < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        mask -= 8;
    }

    val = inb(port) | (1 << mask);
    outb(port, val);
}

void pic_unmask(uint32_t mask)
{
    uint8_t val;
    uint16_t port;

    if (mask < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        mask -= 8;
    }

    val = inb(port) & ~(1 << mask);
    outb(port, val);
}

void pic_eoi(uint8_t irq)
{
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_init()
{
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, PIC1_OFFSET);
    outb(PIC2_DATA, PIC2_OFFSET);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Mask all IRQs
    outb(PIC1_DATA, 0xFB);
    outb(PIC2_DATA, 0xFF);

    dbgprintf("Initialized PIC: offset1=0x%x offset2=0x%x\n", PIC1_OFFSET, PIC2_OFFSET);

    sti();
}

void pic_disable()
{
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}
