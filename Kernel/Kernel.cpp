/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Boot/multiboot.h>
#include <Kernel/CPU/GDT.h>
#include <Kernel/CPU/IDT.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/DebugConsole.h>
#include <Kernel/Devices/CMOS.h>
#include <Kernel/Devices/KeyboardDevice.h>
#include <Kernel/Devices/PIT.h>
#include <Kernel/Devices/VirtualConsole.h>
#include <Kernel/Filesystem/VFS.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Network/E1000NetworkCard.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/panic.h>
#include <Universal/Logger.h>
#include <Universal/StringView.h>
#include <Universal/Types.h>

#if !defined(__os__)
#    error "Compiling with incorrect toolchain."
#endif

VirtualConsole* tty0;

[[noreturn]] void simple_process_runnable1()
{
    // dbgprintf("Kernel", "Running a simple process 1!\n");
    asm volatile("jmp $");
    while (true) { }
}

[[noreturn]] void simple_process_runnable2()
{
    dbgprintf("Kernel", "Running a simple process 2!\n");
    while (true) { }
}

[[noreturn]] static void kernel_main()
{
    // TODO: Only text-mode is supported currently
    // GraphicsManager::the().init();

    KeyboardDevice::the();

    tty0 = new VirtualConsole(0);
    tty0->set_focused(true);

    VFS::the().init();

    auto card = Network::E1000NetworkCard::detect();

    Process::create_user_process("/bin/shell", 0, tty0);
    // Process::create_user_process("/bin/id", 0, tty0);

    Time boot_time = CMOS::boot_time();

    dbgprintf("Kernel", "Boot date: %s %s %d %d:%d\n", boot_time.day_string(), boot_time.month_string(), boot_time.day(), boot_time.hour(), boot_time.minute());
    dbgprintf("Kernel", "Operating System booted!\n");

    while (true)
        ;
}

extern "C" [[noreturn]] void kernel_entry(u32* boot_page_directory, const multiboot_information_t* multiboot, const u32 magicNumber)
{
    DebugConsole::the().enable();

    if (magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("Multiboot header is malformed");
    }

    if (multiboot->flags & MULTIBOOT_FLAGS_BOOTLOADER_NAME) {
        dbgprintf("Kernel", "Loaded by: %s\n", multiboot->bootloader_name);
    }

    kmalloc_init();

    GDT::init();

    IDT::init();

    PIC::init();

    PIT::the();

    MemoryManager::init(boot_page_directory, multiboot);

    Process::create_kernel_process("kernel_main", kernel_main);
    // Process::create_kernel_process("simple1", simple_process_runnable1);

    PM.start();

    while (true) {
        asm volatile("hlt");
    }
}
