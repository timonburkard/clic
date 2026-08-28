cmake_minimum_required(VERSION 3.16)

project(clic
    VERSION 0.0.1
    LANGUAGES C
)

add_executable(clic
    src/clic.c
    test/greet.c
)

target_include_directories(clic
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(clic PRIVATE
    -Wall
    -Wextra
    -Wshadow
    -Wconversion
    -Wpedantic
    -Werror
)

set_target_properties(clic PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
