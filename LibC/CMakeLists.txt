SET(LIBC_SOURCES
    src/stdio.cpp
    src/stdlib.cpp
    src/string.cpp
)

set(SOURCES
    ${LIBC_SOURCES}
)

add_library(c STATIC ${SOURCES})
install(TARGETS c DESTINATION LibC)
