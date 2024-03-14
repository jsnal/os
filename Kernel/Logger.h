#pragma once

#include <Kernel/Devices/PIT.h>
#include <Kernel/kprintf.h>
#include <stdarg.h>

#define ENABLE_COLOR

#define DEBUG_KEYBOARD 0
#define DEBUG_PATA_DISK 0
#define DEBUG_PROCESS 0
#define DEBUG_PROCESS_MANAGER 0
#define DEBUG_EXT2_FILESYSTEM 1

#define dbgprintf(tag, format, ...)                                                                                                      \
    do {                                                                                                                                 \
        kprintf(FORMAT_BOLD "[Kernel:%u]:" FORMAT_DEBUG "%s: " FORMAT_RESET format, PIT::milliseconds_since_boot(), tag, ##__VA_ARGS__); \
    } while (0)

#define dbgprintf_if(condition, tag, format, ...) \
    if (condition) {                              \
        dbgprintf(tag, format, ##__VA_ARGS__);    \
    }

#define infoprintf(format, ...)                                                   \
    do {                                                                          \
        kprintf(FORMAT_INFO "%s: " FORMAT_RESET format, __FILE__, ##__VA_ARGS__); \
    } while (0)

#define warnprintf(format, ...)                                                   \
    do {                                                                          \
        kprintf(FORMAT_WARN "%s: " FORMAT_RESET format, __FILE__, ##__VA_ARGS__); \
    } while (0)

#define errprintf(format, ...)                                                   \
    do {                                                                         \
        kprintf(FORMAT_ERR "%s: " FORMAT_RESET format, __FILE__, ##__VA_ARGS__); \
    } while (0)

#ifdef ENABLE_COLOR
#    define FORMAT_DEBUG "\x1b[35;1m\x1b[35m"
#    define FORMAT_INFO "\x1b[35;1m\x1b[34m"
#    define FORMAT_WARN "\x1b[35;1m\x1b[33m"
#    define FORMAT_ERR "\x1b[35;1m\x1b[31m"
#    define FORMAT_ERR "\x1b[35;1m\x1b[31m"
#    define FORMAT_BOLD "\x1b[1m"
#    define FORMAT_RESET "\x1b[0m"
#else
#    define FORMAT_DEBUG ""
#    define FORMAT_INFO ""
#    define FORMAT_WARN ""
#    define FORMAT_ERR ""
#    define FORMAT_RESET ""
#endif
