#pragma once

#include <Universal/Types.h>

namespace IO {

static inline u8 inb(u16 port)
{
    u8 v;
    asm volatile("inb %0, %1"
                 : "=a"(v)
                 : "Nd"(port));
    return v;
}

static inline void outb(u16 port, u8 value)
{
    asm volatile("out %1, %0"
                 :
                 : "a"(value), "Nd"(port));
}

static inline u16 inw(u16 port)
{
    u16 v;
    asm volatile("inw %0, %1"
                 : "=a"(v)
                 : "Nd"(port));
    return v;
}

static inline void outw(u16 port, u16 value)
{
    asm volatile("out %1, %0"
                 :
                 : "a"(value), "Nd"(port));
}

static inline u32 inl(u16 port)
{
    u32 v;
    asm volatile("in %0, %1"
                 : "=a"(v)
                 : "Nd"(port));
    return v;
}

static inline void outl(u16 port, u32 value)
{
    asm volatile("out %1, %0"
                 :
                 : "a"(value), "Nd"(port));
}
}
