#ifndef CPU_PANIC_H
#define CPU_PANIC_H

#include <devices/vga.h>
#include <kprintf.h>
#include <stdio.h>

#define PANIC_BUFFER_SIZE 256

/**
 * @brief freeze the kernel without printing a message
 */
static inline void hang()
{
    asm volatile("cli; \
                  hlt");
}

/**
 * @brief panic the kernel with a message
 */
static inline void panic(const char* format, ...)
{
    char message[PANIC_BUFFER_SIZE];

    va_list ap;
    va_start(ap, format);
    vsnprintf(message, PANIC_BUFFER_SIZE, format, ap);
    va_end(ap);

    vga_printf("*** PANIC ***\n%s", message);
    kprintf("*** PANIC ***\n%s", message);

    hang();
}

#endif
