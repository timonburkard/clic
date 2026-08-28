#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clic.h"

enum {
    ARG_ID_NAME = 0,
    ARG_ID_NUMBER,
    ARG_ID_LOUD
};

static const char* const hello_names[]  = {"hello", NULL};
static const char* const bye_names[]    = {"bye", NULL};
static const char* const number_names[] = {"--number", "-n", NULL};
static const char* const loud_names[]   = {"--loud", "-l", NULL};

static clic_err_t hello(clic_res_t* result);
static clic_err_t bye(clic_res_t* result);

static clic_err_t print_message(clic_res_t* result, const char* message);
static char*      str_toupper(const char* input, char* output);

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
    [ARG_ID_LOUD] = {
        .type        = CLIC_ARG_FLAG,
        .required    = false,
        .names       = loud_names,
        .value_name  = NULL,
        .description = "Shout the greeting loudly",
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
    char* name    = result->argv[ARG_ID_NAME];
    bool  is_loud = result->argv[ARG_ID_LOUD] != NULL;
    int   number  = result->argv[ARG_ID_NUMBER] == NULL ? 1 : atoi(result->argv[ARG_ID_NUMBER]);

    char upper_name[100];

    for (int i = 0; i < number; ++i) {
        printf("%s, %s!\n", message, is_loud ? str_toupper(name, upper_name) : name);
    }

    return CLIC_ERR_OK;
}

static char* str_toupper(const char* input, char* output)
{
    for (size_t i = 0; i < strlen(input); i++) {
        output[i] = (char)toupper(input[i]);
    }

    output[strlen(input)] = '\0';

    return output;
}
