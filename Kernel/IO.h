#pragma once

#include <Universal/Types.h>

namespace IO {

static inline u8 inb(u16 port)
{
    u8 v;
    asm volatile("inb %b0, %w1"
                 : "=a"(v)
                 : "Nd"(port));
    return v;
}

static inline void outb(u16 port, u8 value)
{
    asm volatile("out %w1, %b0"
                 :
                 : "a"(value), "Nd"(port));
}

static inline u16 inw(u16 port)
{
    u16 v;
    asm volatile("inw %w0, %w1"
                 : "=a"(v)
                 : "Nd"(port));
    return v;
}

static inline void outw(u16 port, u16 value)
{
    asm volatile("out %w1, %w0"
                 :
                 : "a"(value), "Nd"(port));
}
}
