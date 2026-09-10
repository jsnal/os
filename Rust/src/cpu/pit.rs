use core::sync::atomic::{AtomicUsize, Ordering};

use crate::{
    cpu::{
        idt::{self, InterruptStackFrame},
        io, pic,
    },
    dbgprintln,
};

const PIT_BASE_FREQUENCY: u32 = 1193182;
const PIT_TICKS_PER_SECOND: u32 = 1000;
const PIT_TIMER_RELOAD: u16 = (PIT_BASE_FREQUENCY / PIT_TICKS_PER_SECOND) as u16;
const PIT_CHANNEL_0: u16 = 0x40;
const PIT_COMMAND: u16 = 0x0043;
const PIT_SELECT_CHANNEL_0: u8 = 0x00;
const PIT_MODE_SQUARE_WAVE: u8 = 0x06;
const PIT_WRITE_WORD: u8 = 0x30;

static UPTIME_MS: AtomicUsize = AtomicUsize::new(0);
static UPTIME_S: AtomicUsize = AtomicUsize::new(0);

fn handle_irq(_: &mut InterruptStackFrame) {
    let uptime_ms = UPTIME_MS.fetch_add(1, Ordering::Relaxed);
    if uptime_ms % (PIT_TICKS_PER_SECOND as usize) == 0 {
        UPTIME_S.fetch_add(1, Ordering::Relaxed);
    }
}

pub fn get_uptime() -> usize {
    UPTIME_S.load(Ordering::SeqCst)
}

/// # Notes:
/// * https://wiki.osdev.org/Programmable_Interval_Timer
pub fn init() {
    // Set channel 0 to use the square wave generator mode
    io::outb(
        PIT_COMMAND,
        PIT_SELECT_CHANNEL_0 | PIT_WRITE_WORD | PIT_MODE_SQUARE_WAVE,
    );

    // Provide channel 0 with a timing that corresponds to a tick every millisecond
    io::outb(PIT_CHANNEL_0, (PIT_TIMER_RELOAD & 0xFF) as u8);
    io::outb(PIT_CHANNEL_0, ((PIT_TIMER_RELOAD >> 8) & 0xFF) as u8);

    // Start receiving PIT interrupts
    idt::set_irq_handler(pic::PIT_IRQ, handle_irq);
    pic::unmask(pic::PIT_IRQ);

    dbgprintln!(
        "initialized PIT: freq={}Hz, mode=square wave",
        PIT_TICKS_PER_SECOND
    );
}
