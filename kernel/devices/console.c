#include "console.h"
#include "api/printf.h"
#include "api/sys/io.h"
#include <stdarg.h>

static bool console_enabled;

bool console_enable_com_port()
{
    if (console_enabled) {
        return true;
    }

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

void console_putchar(char c)
{
    if (!console_enabled) {
        return;
    }

    outb(c, COM1);
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
