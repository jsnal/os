/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Stdlib.h>
#include <Universal/Types.h>
#include <stdarg.h>
#include <stdbool.h>

#define CONVERT_BUFFER_SIZE 64

[[gnu::always_inline]] inline size_t convert_unsigned_number(unsigned long number, char* buffer, size_t size, int base, bool caps)
{
    const char* digits = caps ? "0123456789ABCDEF" : "0123456789abcdef";
    size_t pos = 0;

    do {
        buffer[pos++] = digits[number % base];
        number /= base;
    } while (number != 0 && pos < size);

    return pos;
}

[[gnu::always_inline]] inline size_t convert_signed_number(long number, char* buffer, size_t size, int base, bool caps)
{
    bool negative = false;
    size_t pos = 0;

    if (number < 0) {
        number = -number;
        negative = true;
    }

    pos = convert_unsigned_number(number, buffer, size, base, caps);

    if (negative) {
        buffer[pos++] = '-';
    }
    return pos;
}

[[gnu::always_inline]] inline size_t write_unsigned_number(unsigned long number, char* buffer, int base, bool caps, int width, int max)
{
    size_t written = 0;
    size_t convert_size;
    char convert_buffer[CONVERT_BUFFER_SIZE];

    convert_size = convert_unsigned_number(number, convert_buffer, CONVERT_BUFFER_SIZE, base, caps);

    int number_size = convert_size;
    int spaces_size = width - number_size;
    convert_size--;

    for (int i = 0; i < spaces_size && max > 0; i++, max--) {
        *(buffer + written++) = '0';
    }

    for (int i = 0; i < number_size && max > 0; i++, max--) {
        *(buffer + written++) = convert_buffer[convert_size--];
    }

    return written;
}

[[gnu::always_inline]] inline size_t write_signed_number(long number, char* buffer, int max)
{
    size_t written = 0;
    int convert_size;
    char convert_buffer[CONVERT_BUFFER_SIZE];

    convert_size = convert_signed_number(number, convert_buffer, CONVERT_BUFFER_SIZE, 10, false) - 1;

    for (; convert_size >= 0 && max > 0; convert_size--, max--) {
        *(buffer + written++) = convert_buffer[convert_size];
    }

    return written;
}

[[gnu::always_inline]] inline size_t write_string(const char* string, char* buffer, int max)
{
    int string_length = strlen(string);

    if (buffer == nullptr) {
        return 1;
    }

    if (string_length > max) {
        string_length = max;
    }

    memcpy(buffer, string, string_length);
    buffer[string_length] = '\0';
    return string_length;
}

[[gnu::always_inline]] inline int printf_buffer(char* str, size_t size, const char* format, va_list ap)
{
    int length = 0;
    int remaining = size;
    unsigned long value_ui;
    signed long value_i;
    char* value_cp;

    for (; *format && remaining > 0; ++format) {
        if (*format == '%') {
            ++format;

            switch (*format) {
                case '%':
                    str[length++] = '%';
                    break;
                case 'c':
                    value_ui = va_arg(ap, unsigned int);
                    str[length++] = (char)value_ui;
                    break;
                case 's':
                    value_cp = va_arg(ap, char*);
                    length += write_string(value_cp, &str[length], remaining);
                    break;
                // TODO: Add width specifier (%08x)
                case 'b':
                    value_ui = va_arg(ap, unsigned int);
                    length += write_unsigned_number(value_ui, &str[length], 2, false, 8, remaining);
                    break;
                case 'x':
                    value_ui = va_arg(ap, unsigned int);
                    length += write_unsigned_number(value_ui, &str[length], 16, false, 8, remaining);
                    break;
                case 'X':
                    value_ui = va_arg(ap, unsigned int);
                    length += write_unsigned_number(value_ui, &str[length], 16, true, 8, remaining);
                    break;
                case 'u':
                    value_ui = va_arg(ap, unsigned long);
                    length += write_unsigned_number(value_ui, &str[length], 10, false, 0, remaining);
                    break;
                case 'd':
                    value_i = va_arg(ap, int);
                    length += write_signed_number(value_i, &str[length], remaining);
                    break;
            }
        } else {
            str[length++] = *format;
        }

        remaining = size - length;
    }

    str[length] = '\0';
    return length;
}
