CC=i686-pc-os-gcc
CFLAGS=-ffreestanding -nostdlib -Wall -Wextra -std=gnu11 -masm=intel -g
LDFLAGS=-ffreestanding -nostdlib
LIBS=-lgcc -I.

CRTBEGIN_OBJ=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

OBJS=\
	api/printf.o \
	api/string.o \
	boot/boot.o \
	boot/crti.o \
	boot/crtn.o \
	cpu/gdt.o \
	cpu/idt.o \
	cpu/lgdt.o \
	cpu/lidt.o \
	cpu/pic.o \
	devices/console.o \
	devices/vga.o \
	kernel.o \
	logger/logger.o \

HEADERS=$(wildcard **/*.h)

LINK_LIST=\
	$(CRTBEGIN_OBJ) \
	$(CRTEND_OBJ) \
	$(OBJS) \

all: kernel.bin

kernel.bin: $(OBJS) linker.ld
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $(LINK_LIST)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(LIBS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) $(LIBS) -c -o $@ $<

%.o: %.s
	$(CC) $(CFLAGS) $(LIBS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f kernel.bin
	rm -f $(OBJS) $(DEPS)
