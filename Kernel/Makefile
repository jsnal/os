NAME = os.kernel

CXX = i686-pc-os-g++
CC = i686-pc-os-gcc
LD = i686-pc-os-ld
AS = i686-pc-os-as

STANDARD_FLAGS = -fno-exceptions -fno-rtti -std=c++17 -nostdinc -nostdlib -nostdinc++ -nodefaultlibs -ffreestanding -masm=intel
WARNING_FLAGS = -Wall -Wextra -Wundef -Wcast-qual
DEFINE_FLAGS = -DKERNEL
OTHER_FLAGS = -xc++

CXXFLAGS = -MMD -MP $(STANDARD_FLAGS) $(WARNING_FLAGS) $(DEFINE_FLAGS) $(OTHER_FLAGS)
LDFLAGS = -Map os.kernel.map --build-id=none
LIBS = -I. -I.. -I../LibC/include

CRTBEGIN_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

CXX_OBJS = \
	CPU/GDT.o \
	CPU/IDT.o \
	CPU/PIC.o \
	CPU/lgdt.o \
	CPU/lidt.o \
	Devices/Console.o \
	Devices/Keyboard.o \
	Devices/PIT.o \
	Devices/VGA.o \
	Kernel.o \
	Memory/MemoryManager.o \
	Memory/PMM.o \
	Stdlib.o \
	kprintf.o \
#	memory/heap.o \
#	memory/memory.o \
#	memory/paging.o \
#	memory/pmm.o \
#	memory/zone.o \

BOOT_OBJS = \
	Boot/boot.o \
	Boot/crti.o \
	Boot/crtn.o \

OBJS = $(CXX_OBJS) $(BOOT_OBJS)
DEPS = $(OBJS:%.o=%.d)

LINK_LIST = \
	$(CRTBEGIN_OBJ) \
	$(CRTEND_OBJ) \
	$(OBJS) \

all: $(NAME)

$(NAME): $(OBJS) linker.ld
	@echo "LD $@"; $(LD) $(LDFLAGS) -T linker.ld -o $@ $(LINK_LIST)

%.o: %.cpp
	@echo "CXX $<"; $(CXX) $(CXXFLAGS) $(LIBS) -c -o $@ $<

%.o: %.S
	@echo "AS $<"; $(AS) -o $@ $<

-include $(DEPS)

.PHONY: clean
clean:
	@echo "CLEAN $(NAME) $(OBJS) $(DEPS)"; rm -f $(NAME) $(OBJS) $(DEPS)
