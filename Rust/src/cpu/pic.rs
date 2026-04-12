use crate::{cpu::io, dbgprintln};

pub const PIT_IRQ: u8 = 0;

const PIC1_COMMAND: u16 = 0x0020;
const PIC1_DATA: u16 = 0x0021;
const PIC2_COMMAND: u16 = 0x00A0;
const PIC2_DATA: u16 = 0x00A1;
pub const PIC1_OFFSET: u8 = 0x20;
const PIC2_OFFSET: u8 = 0x28;
const PIC_EOI: u8 = 0x20;
const ICW1_ICW4: u8 = 0x01;
const ICW1_INIT: u8 = 0x10;
const ICW4_8086: u8 = 0x01;
const PIC_READ_ISR: u8 = 0x0B;
const PIC_CASCADE_IRQ: u8 = 2;

/// # Notes
/// * https://wiki.osdev.org/8259_PIC#Masking
pub fn mask(irq: u8) {
    let (port, mask) = if irq < 8 {
        (PIC1_DATA, irq)
    } else {
        (PIC2_DATA, irq - 8)
    };

    let val = io::inb(port) | (1 << mask);
    io::outb(port, val);
}

/// # Notes
/// * https://wiki.osdev.org/8259_PIC#Masking
pub fn unmask(irq: u8) {
    let (port, mask) = if irq < 8 {
        (PIC1_DATA, irq)
    } else {
        (PIC2_DATA, irq - 8)
    };

    let val = io::inb(port) & !(1 << mask);
    io::outb(port, val);
}

/// # Notes
/// * https://wiki.osdev.org/8259_PIC#End_of_Interrupt
pub fn eoi(irq: u8) {
    if irq >= 8 {
        io::outb(PIC2_COMMAND, PIC_EOI);
    }
    io::outb(PIC1_COMMAND, PIC_EOI);
}

/// # Notes
/// * https://wiki.osdev.org/8259_PIC#ISR_and_IRR
pub fn read_isr() -> u16 {
    io::outb(PIC1_COMMAND, PIC_READ_ISR);
    io::outb(PIC2_COMMAND, PIC_READ_ISR);
    ((io::inb(PIC2_COMMAND) as u16) << 8) | (io::inb(PIC1_COMMAND) as u16)
}

/// # Notes
/// * https://wiki.osdev.org/8259_PIC#Initialisation
pub fn init() {
    // Start the initialization sequence in cascase mode
    io::outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io::outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    // Set the master and slave vector PIC offsets. All interrupts will start at 32 which is right
    // after the CPU's exception interrupts
    io::outb(PIC1_DATA, PIC1_OFFSET);
    io::outb(PIC2_DATA, PIC2_OFFSET);

    // Connect the master PIC to the slave PIC by providing its IRQ number
    io::outb(PIC1_DATA, 1 << PIC_CASCADE_IRQ);

    // Set the slave PIC to use the same IRQ number provided to the master
    io::outb(PIC2_DATA, PIC_CASCADE_IRQ);

    // Set the master and slave to be in 8086 mode
    io::outb(PIC1_DATA, ICW4_8086);
    io::outb(PIC2_DATA, ICW4_8086);

    // Mask all possible IRQs to start
    io::outb(PIC1_DATA, 0xFB);
    io::outb(PIC2_DATA, 0xFF);

    // Now it's safe to enable interrupts without getting flooded
    super::sti();

    dbgprintln!(
        "loaded PIC: master={:#08x}, slave={:#08x}",
        PIC1_OFFSET,
        PIC2_OFFSET
    )
}
