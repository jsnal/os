#ifndef CPU_PANIC_H
#define CPU_PANIC_H

#include <devices/vga.h>
#include <logger.h>

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
static inline void panic(const char* message)
{
    vga_printf("*** PANIC ***\n%s", message);
    errprintf("*** PANIC ***\n%s", message);
    hang();
}

#endif
