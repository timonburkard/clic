function(add_greet_test name expected_exit expected_file)
    set(test_command
        ${CMAKE_COMMAND}
        "-DGREET_EXECUTABLE=$<TARGET_FILE:greet>"
        "-DEXPECTED_EXIT=${expected_exit}"
        "-DEXPECTED_FILE=${CMAKE_CURRENT_SOURCE_DIR}/expected/${expected_file}"
    )

    set(argument_index 0)
    foreach(argument IN LISTS ARGN)
        list(APPEND test_command "-DARG_${argument_index}=${argument}")
        math(EXPR argument_index "${argument_index} + 1")
    endforeach()

    list(APPEND test_command
        "-DARGUMENT_COUNT=${argument_index}"
        -P
        ${CMAKE_CURRENT_SOURCE_DIR}/run_cli_test.cmake
    )

    add_test(NAME ${name} COMMAND ${test_command})
endfunction()

add_greet_test(greet_hello_default 0 hello_default.txt hello Alice)
add_greet_test(greet_hello_number_long 0 hello_three.txt hello Alice --number 3)
add_greet_test(greet_hello_number_short 0 hello_three.txt hello Alice -n 3)
add_greet_test(greet_bye_default 0 bye_default.txt bye Alice)
add_greet_test(greet_bye_number_long 0 bye_two.txt bye Alice --number 2)
add_greet_test(greet_bye_number_short 0 bye_two.txt bye Alice -n 2)

add_greet_test(greet_help_long 0 help.txt --help)
add_greet_test(greet_help_short 0 help.txt -h)
add_greet_test(greet_hello_help 0 hello_help.txt hello --help)
add_greet_test(greet_bye_help 0 bye_help.txt bye --help)
add_greet_test(greet_version_long 0 version.txt --version)
add_greet_test(greet_version_short 0 version.txt -V)

add_greet_test(greet_missing_command 1 missing_command.txt)
add_greet_test(greet_unknown_command 1 missing_command.txt unknown)
add_greet_test(greet_hello_missing_name 1 hello_missing_name.txt hello)
add_greet_test(greet_bye_missing_name 1 bye_missing_name.txt bye)
add_greet_test(greet_number_missing_value 1 hello_missing_name.txt hello Alice --number)
add_greet_test(greet_unknown_option 1 hello_missing_name.txt hello Alice --unknown)
