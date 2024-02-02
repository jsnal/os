NAME = os.kernel

CC = i686-pc-os-gcc
LD = i686-pc-os-ld
AS = i686-pc-os-as

STANDARD_FLAGS = -std=gnu11 -nostdlib -ffreestanding -masm=intel
WARNING_FLAGS = -Wall -Wextra -Wundef -Wcast-qual

CCFLAGS = -MMD -MP $(STANDARD_FLAGS) $(WARNING_FLAGS)
LDFLAGS = -Map kernel.bin.map
LIBS = -lgcc -I.

CRTBEGIN_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

CC_OBJS = \
	api/printf.o \
	api/string.o \
	cpu/gdt.o \
	cpu/idt.o \
	cpu/lgdt.o \
	cpu/lidt.o \
	cpu/panic.o \
	cpu/pic.o \
	devices/console.o \
	devices/vga.o \
	kernel.o \
	logger/logger.o \

BOOT_OBJS = \
	boot/boot.o \
	boot/crti.o \
	boot/crtn.o \

OBJS = $(CC_OBJS) $(BOOT_OBJS)

LINK_LIST = \
	$(CRTBEGIN_OBJ) \
	$(CRTEND_OBJ) \
	$(OBJS) \

all: $(NAME)

$(NAME): $(OBJS) linker.ld
	@echo "LD $@";
	$(LD) $(LDFLAGS) -T linker.ld -o $@ $(LINK_LIST)

%.o: %.c
	@echo "CC $<"; $(CC) $(CCFLAGS) $(LIBS) -c -o $@ $<

%.o: %.S
	@echo "AS $@"; $(AS) -o $@ $<

-include $(OBJS:%.o=%.d)

.PHONY: clean
clean:
	@echo "CLEAN kernel.bin $(OBJS) $(DEPS)"; rm -f kernel.bin *.d **/*.d $(OBJS) $(DEPS)
