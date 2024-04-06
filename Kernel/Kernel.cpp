/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Boot/multiboot.h>
#include <Kernel/CPU/GDT.h>
#include <Kernel/CPU/IDT.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/Devices/Console.h>
#include <Kernel/Devices/Keyboard.h>
#include <Kernel/Devices/PATADisk.h>
#include <Kernel/Devices/PIT.h>
#include <Kernel/Devices/VGA.h>
#include <Kernel/Filesystem/Ext2Filesystem.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Spinlock.h>
#include <Kernel/panic.h>
#include <Universal/Bitmap.h>
#include <Universal/Types.h>

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
    auto disk = PATADisk::create(PATADisk::Secondary, PATADisk::Master);

    if (disk.ptr() == nullptr) {
        panic("Unable to find Disk!\n");
    }

    auto ext2_filesystem = Ext2Filesystem(disk.leak_ptr());
    if (ext2_filesystem.init().is_error()) {
        panic("Unable to initialize root filesystem\n");
    }

    auto inode1 = ext2_filesystem.inode(12);
    dbgprintf("Kernel", "type: %x\n", inode1->data().mode);
    dbgprintf("Kernel", "size: %d\n", inode1->data().size);
    u8* buffer = new u8[4096];
    inode1->read(0, 30, buffer);
    dbgprintf("Kernel", "file contents: %s\n", buffer);

    while (true) {
    }
}

[[noreturn]] static void kernel_main()
{

    PM.create_kernel_process(simple_process_runnable1, "simple1");
    PM.create_kernel_process(simple_process_runnable2, "simple2");
    PM.create_kernel_process(simple_process_runnable3, "simple3");

    Keyboard::the();

    vga_write("System Booted!");

    PM.init();

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

    PIT::the();

    vga_init();

    MemoryManager::the().init(boot_page_directory, multiboot);

    kernel_main();

    while (true) {
        asm volatile("hlt");
    }
}
