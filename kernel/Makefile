CC=i686-elf-gcc

CFLAGS=-ffreestanding -nostdlib -Wall -Wextra -O2 -g
LIBS=-nostdlib -lgcc

KERNEL_OBJS=\
						kernel.o \

LINK_LIST=\
					boot/boot.o \
					$(KERNEL_OBJS) \

os.bin: $(KERNEL_OBJS) boot/boot.o linker.ld
	$(CC) -T linker.ld -o $@ $(CFLAGS) $(LINK_LIST)

.c.o:
	$(CC) -c $< -o $@ -std=c99 $(CFLAGS)

boot/boot.o: boot/boot.s
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f os.bin
	rm -f $(LINK_LIST)
