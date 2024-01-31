#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <stdarg.h>

#define ENABLE_COLOR true

#define dbgln(format, ...)                        \
    do {                                          \
        debug_line(DEBUG, format, ##__VA_ARGS__); \
    } while (0)
#define infoln(format, ...)                      \
    do {                                         \
        debug_line(INFO, format, ##__VA_ARGS__); \
    } while (0)
#define warnln(format, ...)                      \
    do {                                         \
        debug_line(WARN, format, ##__VA_ARGS__); \
    } while (0)
#define errln(format, ...)                        \
    do {                                          \
        debug_line(ERROR, format, ##__VA_ARGS__); \
    } while (0)

#ifdef ENABLE_COLOR
#    define DEBUG_FORMAT "\x1b[35;1m\x1b[35m[DEBUG]\x1b[0m "
#    define INFO_FORMAT "\x1b[35;1m\x1b[34m[INFO]\x1b[0m "
#    define WARN_FORMAT "\x1b[35;1m\x1b[33m[WARN]\x1b[0m "
#    define ERROR_FORMAT "\x1b[35;1m\x1b[31m[ERROR]\x1b[0m "
#else
#    define DEBUG_FORMAT "[DEBUG] "
#    define INFO_FORMAT "[INFO] "
#    define WARN_FORMAT "[WARN] "
#    define ERROR_FORMAT "[ERROR] "
#endif

typedef enum log_level_enum {
    DEBUG,
    INFO,
    WARN,
    ERROR
} log_level_t;

void debug_line(log_level_t, const char* format, ...);

#endif
