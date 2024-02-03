NAME = os.kernel

CC = i686-pc-os-gcc
LD = i686-pc-os-ld
AS = i686-pc-os-as

STANDARD_FLAGS = -g -std=gnu11 -nostdinc -nostdlib -nodefaultlibs -ffreestanding -masm=intel
WARNING_FLAGS = -Wall -Wextra -Wundef -Wcast-qual

CCFLAGS = -MMD -MP $(STANDARD_FLAGS) $(WARNING_FLAGS)
LDFLAGS = # -Map kernel.bin.map
LIBS = -lgcc -I. -I../libc/include

CRTBEGIN_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

LIBC = ../libc/libc.a

CC_OBJS = \
	cpu/gdt.o \
	cpu/idt.o \
	cpu/lgdt.o \
	cpu/lidt.o \
	cpu/panic.o \
	cpu/pic.o \
	devices/console.o \
	devices/pit.o \
	devices/vga.o \
	kernel.o \
	kprintf.o \

LIBC_OBJS = \
	../libc/src/string/memcpy.o

BOOT_OBJS = \
	boot/boot.o \
	boot/crti.o \
	boot/crtn.o \

OBJS = $(CC_OBJS) $(BOOT_OBJS)
DEPS = $(OBJS:%.o=%.d)

LINK_LIST = \
	$(CRTBEGIN_OBJ) \
	$(CRTEND_OBJ) \
	$(OBJS) \
	$(LIBC) \

all: $(NAME)

$(NAME): $(OBJS) $(LIBC) linker.ld
	@echo "LD $@"; $(LD) $(LDFLAGS) -T linker.ld -o $@ $(LINK_LIST)

$(LIBC):
	@echo "MAKE libc"; $(MAKE) -C ../libc

%.o: %.c
	@echo "CC $<"; $(CC) $(CCFLAGS) $(LIBS) -c -o $@ $<

%.o: %.S
	@echo "AS $@"; $(AS) -o $@ $<

-include $(DEPS)

.PHONY: clean
clean:
	@echo "CLEAN $(NAME) $(OBJS) $(DEPS)"; rm -f $(NAME) $(OBJS) $(DEPS)
