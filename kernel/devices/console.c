#include "console.h"
#include <stdarg.h>
#include <sys/io.h>

static bool console_enabled;

bool console_enable_com_port()
{
    if (console_enabled) {
        return true;
    }

    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
    outb(COM1 + 4, 0x1E);
    outb(COM1 + 0, 0xAE);

    if (inb(COM1 + 0) != 0xAE) {
        return false;
    }

    outb(COM1 + 4, 0x0F);
    console_enabled = true;
    return true;
}

void console_putchar(char c)
{
    if (!console_enabled) {
        return;
    }

    while ((inb(LINE_STATUS) & 0x20) == 0)
        ;

    outb(0xe9, c);
    outb(COM1, c);
}

void console_write(const char* str, int length)
{
    for (int i = 0; i < length; i++) {
        console_putchar(str[i]);
    }
}
