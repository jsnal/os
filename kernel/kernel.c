#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/pic.h>
#include <devices/console.h>
#include <devices/keyboard.h>
#include <devices/pit.h>
#include <devices/vga.h>
#include <logger.h>
#include <memory/memory.h>
#include <memory/paging.h>
#include <multiboot.h>
#include <panic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__os__)
#    error "Compiling with incorrect toolchain."
#endif

#define DEBUG_TAG "Kernel"

void kernel_main()
{
    vga_printf("System booted!!\n");

    keyboard_init();

    for (;;)
        asm("hlt");
}

void kernel_entry(uint32_t* boot_page_directory, const multiboot_information_t* multiboot, const uint32_t magicNumber)
{
    console_enable_com_port();

    if (magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("Multiboot header is malformed");
    }

    if (multiboot->flags & MULTIBOOT_FLAGS_BOOTLOADER_NAME) {
        dbgprintf("Loaded by: %s\n", multiboot->bootloader_name);
    }

    gdt_init();

    pic_init();

    idt_init();

    vga_init();

    pit_init();

    init_memory(boot_page_directory, multiboot);

    // init_paging(boot_page_directory, multiboot);

    kernel_main();
}
