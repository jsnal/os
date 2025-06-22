set(LIBC_SOURCES
    MallocManager.cpp
    crt0.cpp
    dirent.cpp
    fcntl.cpp
    stdio.cpp
    stdlib.cpp
    string.cpp
    sys/ioctl.cpp
    sys/mman.cpp
    sys/stat.cpp
    sys/syscall.cpp
    sys/wait.cpp
    termios.cpp
    unistd.cpp
)

add_library(c STATIC ${LIBC_SOURCES} ${UNIVERSAL_SOURCES})
install(TARGETS c DESTINATION LibC)
