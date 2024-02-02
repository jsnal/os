#include "console.h"
#include "api/printf.h"
#include <api/sys/io.h>
#include <stdarg.h>

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

    while ((inb(0x3FD) & 0x20) == 0)
        ;

    outb(COM1, c);
}

int console_printf(const char* format, ...)
{
    va_list list;
    int i;

    va_start(list, format);
    i = vsprintf(console_putchar, format, list);
    va_end(list);
    return i;
}
