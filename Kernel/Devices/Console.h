#pragma once

#include <stdbool.h>
#include <sys/types.h>

class Console final {
public:
    static Console& the();

    Console();

    bool enable();

    void put_char(char);

    void write(const char* string, size_t length);

private:
    bool m_is_enabled { false };
};
