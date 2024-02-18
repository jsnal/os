SET(KERNEL_SOURCES
    CPU/GDT.cpp
    CPU/IDT.cpp
    CPU/PIC.cpp
    CPU/lgdt.S
    CPU/lidt.S
    Devices/Console.cpp
    Devices/Keyboard.cpp
    Devices/PIT.cpp
    Devices/VGA.cpp
    Kernel.cpp
    Memory/MemoryManager.cpp
    Memory/PMM.cpp
    Memory/Zone.cpp
    Runtime.cpp
    Stdlib.cpp
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

add_link_options(-T ${CMAKE_SOURCE_DIR}/Kernel/linker.ld -nostdlib --build-id=none)

add_executable(Kernel ${SOURCES})
target_link_libraries(Kernel gcc)
install(TARGETS Kernel RUNTIME DESTINATION Kernel)
