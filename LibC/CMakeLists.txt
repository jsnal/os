SET(LIBC_SOURCES
    ../Universal/PrintFormat.cpp
    ../Universal/Stdlib.cpp
    crt0.cpp
    stdio.cpp
    stdlib.cpp
    string.cpp
    sys/ioctl.cpp
    sys/syscall.cpp
    termios.cpp
    unistd.cpp
)

set(SOURCES
    ${LIBC_SOURCES}
)

add_library(c STATIC ${SOURCES})
install(TARGETS c DESTINATION LibC)
