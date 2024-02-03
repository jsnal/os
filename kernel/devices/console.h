#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>

#define COM1 0x3f8
#define LINE_STATUS 0x3FD

/**
 * @brief enable the console device serial connection
 *
 * @return true if successful, false otherwise
 */
bool console_enable_com_port();

/**
 * @brief put a character to the console device if it's enabled
 *
 * @param c character to put
 */
void console_putchar(char c);

/**
 * @brief write a string of characters to the console
 *
 * @param str the string to write
 * @param length the length of the string to write
 */
void console_write(const char* str, int length);

#endif
