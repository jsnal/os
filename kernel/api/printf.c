#include "api/printf.h"
#include <api/string.h>
#include <devices/vga.h>
#include <stddef.h>

static size_t intlen(const long num)
{
  if (num == 0) {
    return 1;
  }

  long n = num;
  size_t len = 0;

  if (n < 0) {
    len++;
    n *= -1;
  }

  for (; n > 0; len++) {
    n = n / 10;
  }

  return len;
}

int vsprintf(void (*write)(char), const char *format, va_list list)
{
  size_t length;
  long num;
  const char *str;

  for (; *format; ++format) {
    if (*format == '%') {
      ++format;

      switch (*format) {
        case '%':
          write('%');
          break;
        case 's':
          str = va_arg(list, char*);
          length = strlen(str);

          for (int i = 0; i < length; i++) {
            write(str[i]);
          }

          break;
        case 'd':
          num = va_arg(list, int);
          length = intlen(num) + 1;

          char b[length];
          itoa(b, length, num, 10);

          for (int i = 0; i < length - 1; i++) {
            write(b[i]);
          }
      }
    } else {
      write(*format);
    }
  }
}

int printf_vga(const char *format, ...)
{
  va_list list;
  int i;

  va_start(list, format);
  i = vsprintf(vga_putchar, format, list);
  va_end(list);
  return i;
}
