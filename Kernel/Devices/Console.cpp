#include <Kernel/Devices/Console.h>
#include <Kernel/IO.h>
#include <stdarg.h>

#define COM1 0x3f8
#define LINE_STATUS 0x3FD

// Print out to Bochs console through 0xE9
#define CONSOLE_ENABLE_BOCHS

Console& Console::the()
{
    static Console s_the;
    return s_the;
}

Console::Console()
{
}

bool Console::enable()
{
    IO::outb(COM1 + 1, 0x00);
    IO::outb(COM1 + 3, 0x80);
    IO::outb(COM1 + 0, 0x03);
    IO::outb(COM1 + 1, 0x00);
    IO::outb(COM1 + 3, 0x03);
    IO::outb(COM1 + 2, 0xC7);
    IO::outb(COM1 + 4, 0x0B);
    IO::outb(COM1 + 4, 0x1E);
    IO::outb(COM1 + 0, 0xAE);

    if (IO::inb(COM1 + 0) != 0xAE) {
        return false;
    }

    IO::outb(COM1 + 4, 0x0F);
    m_is_enabled = true;
    return true;
}

void Console::put_char(char c)
{
    if (!m_is_enabled) {
        return;
    }

    while ((IO::inb(LINE_STATUS) & 0x20) == 0)
        ;

#ifdef CONSOLE_ENABLE_BOCHS
    IO::outb(0xe9, c);
#endif

    IO::outb(COM1, c);
}

void Console::write(const char* string, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        put_char(string[i]);
    }
}
