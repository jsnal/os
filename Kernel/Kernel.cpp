#include <Kernel/Boot/multiboot.h>
#include <Kernel/CPU/GDT.h>
#include <Kernel/CPU/IDT.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/Devices/Console.h>
#include <Kernel/Devices/Keyboard.h>
#include <Kernel/Devices/PIT.h>
#include <Kernel/Devices/VGA.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Storage/ATA.h>
#include <Kernel/panic.h>
#include <Universal/Bitmap.h>
#include <Universal/Types.h>

#if !defined(__os__)
#    error "Compiling with incorrect toolchain."
#endif

[[noreturn]] void kernel_main()
{

    Keyboard::the().init();

    vga_write("System Booted!");

    for (;;)
        asm volatile("hlt");
}

extern "C" [[noreturn]] void kernel_entry(u32* boot_page_directory, const multiboot_information_t* multiboot, const u32 magicNumber)
{
    Console::the().enable();

    if (magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("Multiboot header is malformed");
    }

    if (multiboot->flags & MULTIBOOT_FLAGS_BOOTLOADER_NAME) {
        dbgprintf("Kernel", "Loaded by: %s\n", multiboot->bootloader_name);
    }

    GDT::init();

    IDT::init();

    PIC::init();

    PIT::init();

    vga_init();

    MemoryManager::the().init(boot_page_directory, multiboot);

    Process::init();
    // Storage::ATA::init();

    kernel_main();
}
