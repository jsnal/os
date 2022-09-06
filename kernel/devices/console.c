#include "console.h"

bool console_enable()
{
    outb(0x00, COM1 + 1);
    outb(0x80, COM1 + 3);
    outb(0x03, COM1 + 0);
    outb(0x00, COM1 + 1);
    outb(0x03, COM1 + 3);
    outb(0xC7, COM1 + 2);
    outb(0x0B, COM1 + 4);
    outb(0x1E, COM1 + 4);
    outb(0xAE, COM1 + 0);

    if (inb(COM1 + 0) != 0xAE) {
        return false;
    }

    outb(0x0F, COM1 + 4);
    console_enabled = true;
    return true;
}

bool console_putchar(char c)
{
    if (!console_enabled) {
        return false;
    }

    outb(c, COM1);
    return true;
}