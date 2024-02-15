#pragma once

#include <stdint.h>

namespace IO {

static inline unsigned char inb(uint16_t port)
{
    unsigned char v;
    asm volatile("inb %b0, %w1"
                 : "=a"(v)
                 : "Nd"(port));
    return v;
}

static inline void outb(uint16_t port, uint8_t value)
{
    asm volatile("out %w1, %b0"
                 :
                 : "a"(value), "Nd"(port));
}

}
