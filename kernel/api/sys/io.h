#ifndef SYS_IO_H
#define SYS_IO_H

static inline unsigned char inb(unsigned short port)
{
    unsigned char v;
    asm volatile("inb %w1, %0"
                 : "=a"(v)
                 : "Nd"(port));
    return v;
}

static inline void outb(unsigned char value, unsigned short port)
{
    asm volatile("outb %b0, %w1"
                 :
                 : "a"(value), "Nd"(port));
}

#endif
