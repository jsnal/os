#ifndef SYS_IO_H
#define SYS_IO_H

static inline unsigned char inb(unsigned short port)
{
    unsigned char v;
    asm volatile("inb %b0, %w1"
                 : "=a"(v)
                 : "Nd"(port));
    return v;
}

static inline void outb(unsigned char value, unsigned short port)
{
    asm volatile("outb %w1, %b0"
                 :
                 : "a"(value), "Nd"(port));
}

static inline void io_wait()
{
    outb(0x80, 0);
}

#endif
