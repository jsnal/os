include_directories(..)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Enable address sanitizer
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

add_compile_definitions(TESTING)
add_subdirectory(Universal)
