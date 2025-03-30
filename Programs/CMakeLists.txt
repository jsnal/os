set(PROGRAM_NAMES
    id
    echo
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lc")

foreach(PROGRAM_NAME ${PROGRAM_NAMES})
    add_executable(${PROGRAM_NAME} ${PROGRAM_NAME}.cpp)
    target_link_libraries(${PROGRAM_NAME} PUBLIC c)
    add_dependencies(${PROGRAM_NAME} c)
    install(TARGETS ${PROGRAM_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/Programs/bin)
endforeach()

add_subdirectory(shell)
