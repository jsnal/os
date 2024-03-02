#include <Kernel/Boot/multiboot.h>
#include <Kernel/CPU/GDT.h>
#include <Kernel/CPU/IDT.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/Devices/Console.h>
#include <Kernel/Devices/Keyboard.h>
#include <Kernel/Devices/PATA.h>
#include <Kernel/Devices/PIT.h>
#include <Kernel/Devices/VGA.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Spinlock.h>
#include <Kernel/panic.h>
#include <Universal/Bitmap.h>
#include <Universal/Types.h>

#include <Kernel/Bus/PCI.h>

#if !defined(__os__)
#    error "Compiling with incorrect toolchain."
#endif

[[noreturn]] void simple_process_runnable1()
{
    dbgprintf("Kernel", "Running a simple process 1!\n");
    while (true) {
    }
}

[[noreturn]] void simple_process_runnable2()
{
    dbgprintf("Kernel", "Running a simple process 2!\n");
    while (true) {
    }
}

[[noreturn]] void simple_process_runnable3()
{
    dbgprintf("Kernel", "Running a simple process 3!\n");
    while (true) {
    }
}

[[noreturn]] static void kernel_main()
{
    ProcessManager::the().create_kernel_process(simple_process_runnable1, "simple1");
    ProcessManager::the().create_kernel_process(simple_process_runnable2, "simple2");
    ProcessManager::the().create_kernel_process(simple_process_runnable3, "simple3");

    // Storage::ATA::init();

    Keyboard::the().init();

    vga_write("System Booted!");

    ProcessManager::the().init();

    while (true)
        ;
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

    u8 value = Bus::PCI::read8({ 0, 0, 0 }, 0xE);
    dbgprintf("Kernel", "PCI Header Type %d\n", value);

    for (u8 slot = 0; slot < 32; slot++) {
        dbgprintf("Kernel", "PCI Vendor ID %d\n", Bus::PCI::read16({ 0, slot, 0 }, 0x0));
    }

    kernel_main();

    while (true) {
        asm volatile("hlt");
    }
}
