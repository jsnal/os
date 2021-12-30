#include "api/printf.h"
#include <stddef.h>
#include <api/string.h>

int vsprintf(void (*write)(char), const char *format, va_list list)
{
  size_t length;
  const char *str;

  for (; *format; ++format) {
    if (*format == '%') {
      ++format;

      switch (*format) {
        case 's':
          str = va_arg(list, char*);
          length = strlen(str);
          /* write(str[0]); */

          /* for (int i = 0; i < length; i++) { */
          /*   write(str[i]); */
          /* } */

          break;
      }
    } else {
      write(*format);
    }
  }
}
