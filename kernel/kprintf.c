#include <kprintf.h>
#include <stdio.h>

#define KPRINTF_BUFFER_SIZE 128

void kvprintf(const char* format, va_list ag)
{
    char msg[KPRINTF_BUFFER_SIZE];
    vsnprintf(msg, KPRINTF_BUFFER_SIZE, format, ag);
}

void kprintf(const char* format, ...)
{
    va_list ap;

    va_start(ap, format);
    kvprintf(format, ap);
    va_end(ap);
}
