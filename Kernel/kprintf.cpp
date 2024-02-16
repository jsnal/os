#include <Devices/Console.h>
#include <LibA/Printf.h>
#include <kprintf.h>

#define KPRINTF_BUFFER_SIZE 128

void kvprintf(const char* format, va_list ag)
{
    char message[KPRINTF_BUFFER_SIZE];
    size_t length;

    length = printf_buffer(message, KPRINTF_BUFFER_SIZE, format, ag);
    if (length > 0) {
        Console::the().write(message, length);
    }
}

void kprintf(const char* format, ...)
{
    va_list ap;

    va_start(ap, format);
    kvprintf(format, ap);
    va_end(ap);
}
