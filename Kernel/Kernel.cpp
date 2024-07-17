/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Boot/multiboot.h>
#include <Kernel/CPU/GDT.h>
#include <Kernel/CPU/IDT.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/Devices/CMOS.h>
#include <Kernel/Devices/Console.h>
#include <Kernel/Devices/KeyboardDevice.h>
#include <Kernel/Devices/PATADisk.h>
#include <Kernel/Devices/PIT.h>
#include <Kernel/Filesystem/Ext2Filesystem.h>
#include <Kernel/Filesystem/VFS.h>
#include <Kernel/Graphics/GraphicsManager.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Spinlock.h>
#include <Kernel/TTY/VirtualConsole.h>
#include <Kernel/panic.h>
#include <Universal/Bitmap.h>
#include <Universal/Types.h>

#if !defined(__os__)
#    error "Compiling with incorrect toolchain."
#endif

[[noreturn]] void simple_process_runnable1()
{
    // dbgprintf("Kernel", "Running a simple process 1!\n");
    asm volatile("jmp $");
    while (true) {
    }
}

[[noreturn]] void simple_process_runnable2()
{
    dbgprintf("Kernel", "Running a simple process 2!\n");
    while (true) {
    }
}

[[noreturn]] static void kernel_main()
{
    GraphicsManager::the().init();

    KeyboardDevice::the();

    VFS::the().init();

    Process::create_user_process("/bin/id", 0, 0);

    Time boot_time = CMOS::boot_time();
    dbgprintf("Kernel", "localtime=%u month=%u day=%u year=%u\n", boot_time.localtime(), boot_time.month(), boot_time.day(), boot_time.year());

    dbgprintf("Kernel", "Operating System booted!\n");

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

    kmalloc_init();

    Console::the().enable_boot_console();

    GDT::init();

    IDT::init();

    PIC::init();

    PIT::the();

    MemoryManager::the().init(boot_page_directory, multiboot);

    Process::create_kernel_process("kernel_main", kernel_main);
    // Process::create_kernel_process("simple1", simple_process_runnable1);

    PM.start();

    while (true) {
        asm volatile("hlt");
    }
}
