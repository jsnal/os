/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stddef.h>
#include <sys/cdefs.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

__BEGIN_DECLS

extern char** environ;

void exit(int status);

int abs(int j);

char* itoa(char* dest, size_t size, int a, int base);

void* malloc(size_t size);
void* calloc(size_t element_count, size_t size);
void free(void* ptr);

char* getenv(const char* name);
int setenv(const char* name, const char* value, int overwrite);

__END_DECLS

#endif
