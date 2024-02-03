#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <kprintf.h>
#include <stdarg.h>

#define ENABLE_COLOR
#define DEBUG

#ifdef DEBUG
#    define dbgprintf(format, ...)                               \
        do {                                                     \
            kprintf(FORMAT_DEBUG "%s: " FORMAT_RESET, __FILE__); \
            kprintf(format, ##__VA_ARGS__);                      \
        } while (0)
#else
#    define dbgprintf(format, ...) \
        do {                       \
        } while (0)
#endif

#define infoprintf(format, ...)                             \
    do {                                                    \
        kprintf(FORMAT_INFO "%s: " FORMAT_RESET, __FILE__); \
        kprintf(format, ##__VA_ARGS__);                     \
    } while (0)
#define warnprintf(format, ...)                             \
    do {                                                    \
        kprintf(FORMAT_WARN "%s: " FORMAT_RESET, __FILE__); \
        kprintf(format, ##__VA_ARGS__);                     \
    } while (0)
#define errprintf(format, ...)                             \
    do {                                                   \
        kprintf(FORMAT_ERR "%s: " FORMAT_RESET, __FILE__); \
        kprintf(format, ##__VA_ARGS__);                    \
    } while (0)

#ifdef ENABLE_COLOR
#    define FORMAT_DEBUG "\x1b[35;1m\x1b[35m"
#    define FORMAT_INFO "\x1b[35;1m\x1b[34m"
#    define FORMAT_WARN "\x1b[35;1m\x1b[33m"
#    define FORMAT_ERR "\x1b[35;1m\x1b[31m"
#    define FORMAT_RESET "\x1b[0m"
#else
#    define FORMAT_DEBUG ""
#    define FORMAT_INFO ""
#    define FORMAT_WARN ""
#    define FORMAT_ERR ""
#    define FORMAT_RESET ""
#endif

#endif
