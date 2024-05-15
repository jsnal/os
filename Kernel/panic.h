#pragma once

#include <Kernel/kprintf.h>
#include <Universal/Printf.h>
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
    printf_buffer(message, PANIC_BUFFER_SIZE, format, ap);
    va_end(ap);

    // vga_printf("*** PANIC ***\n%s", message);
    kprintf("*** PANIC ***\n%s", message);

    hang();
}
