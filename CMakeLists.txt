cmake_minimum_required (VERSION 3.17)
project (os C CXX ASM)

include_directories(.)
add_subdirectory(Tests EXCLUDE_FROM_ALL)
include_directories(LibC/include)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERIFY_INTERFACE_HEADER_SETS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_STAGING_PREFIX ${CMAKE_SOURCE_DIR})
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

set(TOOLCHAIN_PATH ${CMAKE_SOURCE_DIR}/Toolchain/local/bin)
set(TOOLCHAIN_PREFIX ${TOOLCHAIN_PATH}/i686-pc-os-)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}as)
set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}ld)
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}ranlib)
set(CMAKE_STRIP ${TOOLCHAIN_PREFIX}strip)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}ar)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -masm=intel -xc++ -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc -nostdlib -nostdinc++ -nodefaultlibs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sized-deallocation -Wno-address-of-packed-member -Wundef -Wcast-qual -Wwrite-strings -Wimplicit-fallthrough -Wno-nonnull-compare -Wno-deprecated-copy")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffreestanding -fno-sized-deallocation -fno-exceptions -fno-rtti")

set(CMAKE_CXX_LINK_EXECUTABLE
    "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

add_subdirectory(Kernel)
add_subdirectory(LibC)
add_subdirectory(Programs)
