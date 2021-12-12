CC=i686-pc-os-gcc

CFLAGS=-ffreestanding -nostdlib -Wall -Wextra -O2 -g
LIBS=-lgcc -I.

CRTBEGIN_OBJ=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

KERNEL_OBJS=\
						api/string/strlen.o \
						api/string.o \
						kernel.o \

ASM_OBJS=\
				 boot/boot.o \
				 boot/crti.o \
				 boot/crtn.o \

LINK_LIST=\
					$(ASM_OBJS) \
					$(CRTBEGIN_OBJ) \
					$(CRTEND_OBJ) \
					$(KERNEL_OBJS) \

os.bin: $(KERNEL_OBJS) $(ASM_OBJS) linker.ld
	$(CC) -T linker.ld -o $@ $(CFLAGS) $(LINK_LIST)

.c.o:
	$(CC) -c $< -o $@ -std=c99 $(CFLAGS) $(LIBS)

.s.o:
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f os.bin
	rm -f $(ASM_OBJS) $(KERNEL_OBJS)
