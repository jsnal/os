SET(KERNEL_SOURCES
    ../Universal/PrintFormat.cpp
    ../Universal/String.cpp
    ../Universal/Time.cpp
    Bus/PCI.cpp
    CPU/GDT.cpp
    CPU/IDT.cpp
    CPU/IRQHandler.cpp
    CPU/PIC.cpp
    CPU/lgdt.S
    CPU/lidt.S
    DebugConsole.cpp
    Devices/CMOS.cpp
    Devices/Device.cpp
    Devices/KeyboardDevice.cpp
    Devices/PATADisk.cpp
    Devices/PIT.cpp
    Devices/PartitionDevice.cpp
    Devices/RandomDevice.cpp
    Devices/TTYDevice.cpp
    Devices/VirtualConsole.cpp
    Filesystem/Ext2Filesystem.cpp
    Filesystem/Ext2Inode.cpp
    Filesystem/FileDescriptor.cpp
    Filesystem/InodeFile.cpp
    Filesystem/PTSFilesystem.cpp
    Filesystem/PTSInode.cpp
    Filesystem/VFS.cpp
    Graphics/EmulatorVGAGraphicsCard.cpp
    Graphics/GraphicsManager.cpp
    Graphics/VGATextModeGraphicsCard.cpp
    Kernel.cpp
    Memory/AddressAllocator.cpp
    Memory/MemoryManager.cpp
    Memory/PhysicalRegion.cpp
    Memory/VMObject.cpp
    Memory/VirtualRegion.cpp
    Network/E1000NetworkCard.cpp
    Process/ELF.cpp
    Process/Process.cpp
    Process/ProcessManager.cpp
    Process/Syscall.cpp
    Process/WaitingStatus.cpp
    Process/context.S
    Runtime.cpp
    Stdlib.cpp
    kmalloc.cpp
    kprintf.cpp
)

SET(BOOT_SOURCES
    Boot/boot.S
    Boot/crti.S
    Boot/crtn.S
)

set(SOURCES
    ${KERNEL_SOURCES}
    ${BOOT_SOURCES}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKERNEL -ggdb3 -Os")

add_link_options(-T ${CMAKE_SOURCE_DIR}/Kernel/linker.ld -nostdlib --build-id=none -Map=Kernel.map)

add_executable(Kernel ${SOURCES})
add_custom_command(
    TARGET Kernel POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} --only-keep-debug Kernel Kernel.debug
    COMMAND ${CMAKE_OBJCOPY} --strip-debug Kernel
    COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=Kernel.debug Kernel
)
