cmake_minimum_required(VERSION 3.16)

project(clic
    VERSION 0.0.1
    LANGUAGES C
)

add_executable(greet
    src/clic.c
    test/greet.c
)

target_include_directories(greet
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(greet PRIVATE
    "CLIC_NAME=\"greet\""
    "CLIC_DESCRIPTION=\"Simple program to greet someone\""
    "CLIC_VERSION=\"v1.0.0\""
)

target_compile_options(greet PRIVATE
    -Wall
    -Wextra
    -Wshadow
    -Wconversion
    -Wpedantic
    -Werror
)

set_target_properties(greet PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
