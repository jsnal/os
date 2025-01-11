set(LIBC_SOURCES
    crt0.cpp
    stdio.cpp
    stdlib.cpp
    string.cpp
    sys/ioctl.cpp
    sys/syscall.cpp
    termios.cpp
    unistd.cpp
)

add_library(c STATIC ${LIBC_SOURCES} ${UNIVERSAL_SOURCES})
install(TARGETS c DESTINATION LibC)
