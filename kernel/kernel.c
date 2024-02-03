#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/panic.h>
#include <cpu/pic.h>
#include <devices/console.h>
#include <devices/pit.h>
#include <devices/vga.h>
#include <logger.h>
#include <multiboot/multiboot.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__os__)
#    error "Compiling with incorrect toolchain."
#endif

void kernel_main(multiboot_information_t* multiboot)
{
    gdt_init();

    pic_init();

    idt_init();

    vga_init();

    pit_init();

    if (!(multiboot->flags & MULTIBOOT_FLAGS_MMAP)) {
        errprintf("invalid memory map given by GRUB bootloader");
    }

    dbgprintf("System Memory Map:\n");
    dbgprintf("  Lower mem: %d KiB\n", multiboot->memory_lower);
    dbgprintf("  Upper mem: %d KiB\n", multiboot->memory_upper);
    for (uint32_t position = 0, i = 0;
         position < multiboot->memory_map_length;
         position += sizeof(multiboot_mmap_t), i++) {
        multiboot_mmap_t* mmap = multiboot->memory_map + i;

        /* if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) { */
        dbgprintf("  %x%x:%x%x %d (%s)\n", mmap->base_address_high, mmap->base_address_low,
            mmap->length_high, mmap->length_low, mmap->type, mmap->type == 1 ? "available" : "reserved");
        /* } */
    }

    vga_printf("System booted!!\n");

    for (;;)
        asm("hlt");
}

void bootloader_entry(multiboot_information_t* multiboot, uint32_t magicNumber)
{
    console_enable_com_port();

    if (magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("Multiboot header is malformed");
    }

    if (multiboot->flags & MULTIBOOT_FLAGS_BOOTLOADER_NAME) {
        dbgprintf("Loaded by: %s\n", multiboot->bootloader_name);
    }

    kernel_main(multiboot);
}
