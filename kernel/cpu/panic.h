#ifndef CPU_PANIC_H
#define CPU_PANIC_H

/**
 * @brief freeze the kernel without printing a message
 */
static inline void freeze()
{
    asm volatile("cli; \
                  hlt");
}

/**
 * @brief panic the kernel with a message
 */
void panic(const char*);

#endif
