#include "printf.h"
#include <devices/console.h>
#include <devices/vga.h>
#include <stddef.h>
#include <string.h>

static int convert(int value, char* buf, size_t size, int base, int caps)
{
    const char* digits = caps ? "0123456789ABCDEF" : "0123456789abcdef";
    size_t pos = 0;

    /* We return an unterminated buffer with the digits in reverse order. */
    do {
        buf[pos++] = digits[value % base];
        value /= base;
    } while (value != 0 && pos < size);

    return (int)pos;
}

static int print_string(void (*write)(char), const char* str)
{
    int length = 0;
    while (*str) {
        write(*str++);
        length++;
    }

    return length;
}

static int print_int(void (*write)(char), uint32_t number, uint16_t base)
{
    int length = 0;

    // Remember if negative
    bool negative = false;
    if (base == 10) {
        negative = ((int32_t)number) < 0;

        if (negative) {
            number = -number;
        }
    }

    // Write chars in reverse order, not nullterminated
    char revbuf[32];

    char* cbufp = revbuf;
    int len = 0;
    do {
        *cbufp++ = "0123456789abcdef"[number % base];
        ++len;
        number /= base;
    } while (number);

    // If base is 16, write 0's until 8
    if (base == 16) {
        while (len < 8) {
            *cbufp++ = '0';
            ++len;
        }
    }

    // Reverse buffer
    char buf[len + 1];
    for (int i = 0; i < len; i++) {
        buf[i] = revbuf[len - i - 1];
    }
    buf[len] = 0;

    // Print number
    if (negative) {
        write('-');
        length++;
    }
    length += print_string(write, buf);

    return length;
}

int vsprintf(void (*write)(char), const char* format, va_list list)
{
    int length = 0;
    int base = 10;
    long num;
    char ch;
    const char* str;

    for (; *format; ++format) {
        if (*format == '%') {
            ++format;

            switch (*format) {
                case '%':
                    write('%');
                    break;
                case 'c':
                    ch = va_arg(list, int);
                    write(ch);
                    length++;
                    break;
                case 's':
                    str = va_arg(list, char*);
                    length += print_string(write, str);
                    break;
                case 'b':
                case 'x':
                case 'd':
                    num = va_arg(list, int);
                    if (*format == 'b') {
                        base = 2;
                    } else if (*format == 'x') {
                        base = 16;
                    } else {
                        base = 10;
                    }

                    length += print_int(write, num, base);
                    break;
            }
        } else {
            write(*format);
            length++;
        }
    }

    return length;
}

int printf_vga(const char* format, ...)
{
    va_list list;
    int i;

    va_start(list, format);
    i = vsprintf(vga_putchar, format, list);
    va_end(list);
    return i;
}
