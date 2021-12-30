#ifndef API_PRINTF_H
#define API_PRINTF_H

#include <stdarg.h>

int vsprintf(void (*write)(char), const char *format, va_list list);
#endif
