/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <MallocManager.h>
#include <Universal/BasicString.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Universal/Logger.h>

static MallocManager s_malloc_manager;
static ArrayList<String> s_environment;
char** environ;

void exit(int status)
{
    fflush(stdout);
    fflush(stderr);
    _exit(status);
}

int abs(int j)
{
    return (j < 0) ? -j : j;
}

void* malloc(size_t size)
{
    return s_malloc_manager.allocate(size);
}

void* calloc(size_t element_count, size_t size)
{
    return nullptr;
}

void free(void* ptr)
{
    if (ptr == nullptr) {
        return;
    }
    s_malloc_manager.deallocate(ptr);
}

char* getenv(const char* name)
{
    char* value = nullptr;
    for (size_t i = 0; i < s_environment.size(); i++) {
        auto pair = s_environment[i].split('=');
        if (pair.size() != 2) {
            continue;
        }

        if (pair[0] == name) {
            value = pair[1].data();
        }
    }

    return value;
}

int setenv(const char* name, const char* value, int overwrite)
{
    String entry(name);
    entry += "=";
    entry += value;
    s_environment.add_last(entry);
    return 0;
}
