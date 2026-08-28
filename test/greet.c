#include <stdio.h>
#include <stdlib.h>

#include "clic.h"

enum {
    ARG_ID_NAME = 0,
    ARG_ID_NUMBER,
};

static const char* const hello_names[]  = {"hello", NULL};
static const char* const bye_names[]    = {"bye", NULL};
static const char* const number_names[] = {"-n", "--number", NULL};

static clic_err_t hello(clic_res_t* result);
static clic_err_t bye(clic_res_t* result);
static clic_err_t print_message(clic_res_t* result, const char* message);

static const clic_arg_t arguments[] = {
    [ARG_ID_NAME] = {
        .type        = CLIC_ARG_POSITIONAL,
        .required    = true,
        .names       = NULL,
        .value_name  = "NAME",
        .description = "Name to greet",
    },
    [ARG_ID_NUMBER] = {
        .type        = CLIC_ARG_WITH_VALUE,
        .required    = false,
        .names       = number_names,
        .value_name  = "N",
        .description = "Number of greetings",
    },
};

static const clic_cmd_t commands[] = {
    {
        .names       = hello_names,
        .description = "Say hello",
        .function    = hello,
        .argc        = (uint8_t)(sizeof(arguments) / sizeof(arguments[0])),
        .argv        = arguments,
    },
    {
        .names       = bye_names,
        .description = "Say goodbye",
        .function    = bye,
        .argc        = (uint8_t)(sizeof(arguments) / sizeof(arguments[0])),
        .argv        = arguments,
    },
};

int main(int argc, char** argv)
{
    clic_err_t error = clic_parse((uint8_t)(sizeof(commands) / sizeof(commands[0])), commands, argc, (const char* const*)argv);

    return (int)error;
}

static clic_err_t hello(clic_res_t* result)
{
    return print_message(result, "Hello");
}

static clic_err_t bye(clic_res_t* result)
{
    return print_message(result, "Goodbye");
}

static clic_err_t print_message(clic_res_t* result, const char* message)
{
    int number = result->argv[ARG_ID_NUMBER] == NULL ? 1 : atoi(result->argv[ARG_ID_NUMBER]);

    for (int i = 0; i < number; ++i) {
        printf("%s, %s!\n", message, result->argv[ARG_ID_NAME]);
    }

    return CLIC_ERR_OK;
}
