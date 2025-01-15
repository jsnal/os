function(MAKE_PROGRAM PROGRAM_NAME)
    set(SOURCES ${PROGRAM_NAME}.cpp)
    add_executable(${PROGRAM_NAME} ${SOURCES})
    target_link_libraries(${PROGRAM_NAME} PUBLIC c)
    add_dependencies(${PROGRAM_NAME} c)
    install(TARGETS ${PROGRAM_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/Programs/bin)
endfunction()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lc")

MAKE_PROGRAM(id)
MAKE_PROGRAM(shell)
