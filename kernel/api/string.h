#ifndef API_STRING_H
#define API_STRING_H

#include <stddef.h>

#define MAX_ITOA_LEN 64

size_t strlen(const char* str);

void* memset(void* s, int c, size_t n);

void* memcpy(void* dest, const void* src, size_t n);

size_t intlen(const long num);

char* itoa(char* dest, size_t size, int a, int base);

#endif
