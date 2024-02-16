#include <CPU/GDT.h>
#include <CPU/IDT.h>
#include <CPU/PIC.h>
#include <Devices/Console.h>
#include <Devices/Keyboard.h>
#include <Devices/PIT.h>
#include <Devices/VGA.h>
#include <LibA/Bitmap.h>
#include <Logger.h>
#include <multiboot.h>
#include <panic.h>
#include <stdint.h>

#if !defined(__os__)
#    error "Compiling with incorrect toolchain."
#endif

#define DEBUG_TAG "Kernel"

void kernel_main()
{

    Keyboard::the().init();

    vga_write("System Booted!");

    for (;;)
        asm volatile("hlt");
}

extern "C" void kernel_entry(uint32_t* boot_page_directory, const multiboot_information_t* multiboot, const uint32_t magicNumber)
{
    Console::the().enable();

    if (magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("Multiboot header is malformed");
    }

    if (multiboot->flags & MULTIBOOT_FLAGS_BOOTLOADER_NAME) {
        dbgprintf("Loaded by: %s\n", multiboot->bootloader_name);
    }

    GDT::init();

    IDT::init();

    PIC::init();

    PIT::init();

    vga_init();

    kernel_main();
}
