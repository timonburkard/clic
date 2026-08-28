if(NOT DEFINED GREET_EXECUTABLE)
    message(FATAL_ERROR "GREET_EXECUTABLE is required")
endif()

if(NOT DEFINED EXPECTED_EXIT)
    message(FATAL_ERROR "EXPECTED_EXIT is required")
endif()

if(NOT DEFINED EXPECTED_FILE)
    message(FATAL_ERROR "EXPECTED_FILE is required")
endif()

set(test_arguments)

if(ARGUMENT_COUNT GREATER 0)
    math(EXPR last_argument "${ARGUMENT_COUNT} - 1")

    foreach(index RANGE ${last_argument})
        list(APPEND test_arguments "${ARG_${index}}")
    endforeach()
endif()

execute_process(
    COMMAND "${GREET_EXECUTABLE}" ${test_arguments}
    RESULT_VARIABLE actual_exit
    OUTPUT_VARIABLE actual_output
    ERROR_VARIABLE actual_error
)

file(READ "${EXPECTED_FILE}" expected_output)

string(REPLACE "\r\n" "\n" actual_output "${actual_output}")
string(REPLACE "\r\n" "\n" expected_output "${expected_output}")

string(ASCII 27 escape)
string(REGEX REPLACE "${escape}\\[[0-9;]*m" "" actual_output "${actual_output}")

if(NOT actual_exit EQUAL EXPECTED_EXIT)
    message(FATAL_ERROR
        "Unexpected exit code: ${actual_exit}\n"
        "Expected exit code: ${EXPECTED_EXIT}\n"
        "stdout:\n${actual_output}\n"
        "stderr:\n${actual_error}"
    )
endif()

if(NOT actual_error STREQUAL "")
    message(FATAL_ERROR "Unexpected stderr:\n${actual_error}")
endif()

if(NOT actual_output STREQUAL expected_output)
    message(FATAL_ERROR
        "Unexpected stdout.\n"
        "Expected:\n---\n${expected_output}---\n"
        "Actual:\n---\n${actual_output}---"
    )
endif()
