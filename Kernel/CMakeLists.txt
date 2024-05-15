SET(KERNEL_SOURCES
    Bus/PCI.cpp
    CPU/GDT.cpp
    CPU/IDT.cpp
    CPU/IRQHandler.cpp
    CPU/PIC.cpp
    CPU/lgdt.S
    CPU/lidt.S
    Devices/Console.cpp
    Devices/Keyboard.cpp
    Devices/PATADisk.cpp
    Devices/PIT.cpp
    Devices/VGA.cpp
    Filesystem/Ext2Filesystem.cpp
    Filesystem/Inode.cpp
    Graphics/EmulatorVGAGraphicsCard.cpp
    Graphics/VGATextModeGraphicsCard.cpp
    Graphics/GraphicsManager.cpp
    Kernel.cpp
    Memory/AddressAllocator.cpp
    Memory/MemoryManager.cpp
    Memory/PhysicalRegion.cpp
    Memory/VMObject.cpp
    Memory/VirtualRegion.cpp
    Process/Process.cpp
    Process/ProcessManager.cpp
    Process/WaitingStatus.cpp
    Process/context.S
    Runtime.cpp
    Stdlib.cpp
    TTY/VirtualConsole.cpp
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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKERNEL")

add_link_options(-T ${CMAKE_SOURCE_DIR}/Kernel/linker.ld -nostdlib --build-id=none -Map=Kernel.map)

add_executable(Kernel ${SOURCES})
target_link_libraries(Kernel gcc)
install(TARGETS Kernel RUNTIME DESTINATION Kernel)
