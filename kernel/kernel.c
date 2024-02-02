#include <api/printf.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/pic.h>
#include <devices/console.h>
#include <devices/vga.h>
#include <logger/logger.h>
#include <multiboot/multiboot.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <api/sys/io.h>

#if !defined(__os__)
#    error "Compiling with incorrect toolchain."
#endif

void kernel_main(multiboot_information_t* multiboot)
{
    gdt_init();

    idt_init();

    pic_init();

    vga_init();

    if (!(multiboot->flags & MULTIBOOT_FLAGS_MMAP)) {
        errln("invalid memory map given by GRUB bootloader");
    }

    dbgln("System Memory Map:\n");
    for (uint32_t position = 0, i = 0;
         position < multiboot->memory_map_length;
         position += sizeof(multiboot_mmap_t), i++) {
        multiboot_mmap_t* mmap = multiboot->memory_map + i;

        /* if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) { */
        dbgln("  size=%x base_address=%x%x, length=%x%x, type=%x\n",
            mmap->size, mmap->base_address_high, mmap->base_address_low, mmap->length_high, mmap->length_low, mmap->type);
        /* } */
    }

    dbgln("memory_lower=%d KiB, memory_upper=%d KiB\n", multiboot->memory_lower, multiboot->memory_upper);

    printf_vga("Booted!!\n");

    for (;;)
        asm("hlt");
}

void bootloader_entry(multiboot_information_t* multiboot, uint32_t magicNumber)
{
    console_enable_com_port();

    if (magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) {
        asm("hlt");
    }

    if (multiboot->flags & MULTIBOOT_FLAGS_BOOTLOADER_NAME) {
        dbgln("Loaded by: %s\n", multiboot->bootloader_name);
    }

    kernel_main(multiboot);
}
