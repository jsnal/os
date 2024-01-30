#include "logger.h"
#include "api/printf.h"
#include "devices/console.h"

void debug_line(log_level_t log_level, const char* format, ...)
{
    va_list list;

    switch (log_level) {
        case DEBUG:
            console_printf(DEBUG_FORMAT);
            break;
        case INFO:
            console_printf(INFO_FORMAT);
            break;
        case WARN:
            console_printf(WARN_FORMAT);
            break;
        case ERROR:
            console_printf(ERROR_FORMAT);
            break;
    }

    va_start(list, format);
    vsprintf(console_putchar, format, list);
    va_end(list);
}
