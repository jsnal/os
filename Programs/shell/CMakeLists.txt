add_executable(shell
    shell.cpp
    Readline.cpp
)
include_directories(.)
target_link_libraries(shell PUBLIC c)
add_dependencies(shell c)
install(TARGETS shell RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/Programs/bin)
