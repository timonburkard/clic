#include "clic.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Mark code as unreachable
 *
 */
#define UNREACHABLE()                                                               \
    do {                                                                            \
        fprintf(stderr, "UNREACHABLE code reached at %s:%d\n", __FILE__, __LINE__); \
        abort();                                                                    \
    } while (0)

/**
 * @brief Mark code as unreachable with annotation
 *
 */
#define UNREACHABLE_A(text)                                                                    \
    do {                                                                                       \
        fprintf(stderr, "UNREACHABLE code reached at %s:%d | %s\n", __FILE__, __LINE__, text); \
        abort();                                                                               \
    } while (0)

static void version(void);
static void help(uint8_t cmdc, const clic_cmd_t* cmdv, bool is_error);
static void help_cmd(const clic_cmd_t* cmd, bool is_error);
static bool parse_cmd_id(uint8_t cmdc, const clic_cmd_t* cmdv, const char* input, uint8_t* cmd_id);
static bool parse_args(const clic_cmd_t* cmd, int argc, const char* const* argv, clic_res_t* clic_res);
static bool is_config_valid(uint8_t cmdc, const clic_cmd_t* cmdv);

clic_err_t clic_parse(uint8_t cmdc, const clic_cmd_t* cmdv, int argc, const char* const* argv)
{
    clic_res_t clic_res     = {0};
    clic_err_t error        = CLIC_ERR_OK;
    uint8_t    cmd_id       = 0;
    bool       is_cmd_valid = false;

    if ((cmdv == NULL) || (argv == NULL)) {
        return CLIC_ERR_NULL;
    }

    if (!is_config_valid(cmdc, cmdv)) {
        return CLIC_ERR_CONFIG;
    }

    if (argc < 2) {
        help(cmdc, cmdv, true);
        return CLIC_ERR_ARG;
    }

    if ((argc - 2) > CLIC_USER_ARGC_MAX) {
        help(cmdc, cmdv, true);
        return CLIC_ERR_ARG;
    }

    if (CLIC_VERSION_NAMES(argv[1])) {
        version();
        return CLIC_ERR_OK;
    }

    if (CLIC_HELP_NAMES(argv[1])) {
        help(cmdc, cmdv, false);
        return CLIC_ERR_OK;
    }

    is_cmd_valid = parse_cmd_id(cmdc, cmdv, argv[1], &cmd_id);

    if (!is_cmd_valid) {
        help(cmdc, cmdv, true);
        return CLIC_ERR_ARG;
    }

    if ((argc > 2) && CLIC_HELP_NAMES(argv[2])) {
        help_cmd(&cmdv[cmd_id], false);
        return CLIC_ERR_OK;
    }

    if (!parse_args(&cmdv[cmd_id], argc - 2, &argv[2], &clic_res)) {
        help_cmd(&cmdv[cmd_id], true);
        return CLIC_ERR_ARG;
    }

    if (cmdv[cmd_id].function == NULL) {
        return CLIC_ERR_NULL;
    }

    error = cmdv[cmd_id].function(&clic_res);

    if (error == CLIC_ERR_ARG) {
        help_cmd(&cmdv[cmd_id], true);
    }

    return error;
}

static bool parse_cmd_id(uint8_t cmdc, const clic_cmd_t* cmdv, const char* input, uint8_t* cmd_id)
{
    for (uint8_t i = 0; i < cmdc; i++) {
        for (const char* const* name = cmdv[i].names; *name != NULL; name++) {
            if (strcmp(input, *name) == 0) {
                *cmd_id = i;
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief Parse the arguments of a Command
 *
 * @param[in]  cmd      -- Command
 * @param[in]  argc     -- Remaining user input argc after cmd was parsed
 * @param[in]  argv     -- Remaining user input argv after cmd was parsed
 * @param[out] clic_res -- CLIC result

 * @return true  -- Arguments where parsed successfully
 * @return false -- Arguments where not parsed successfully
 */
static bool parse_args(const clic_cmd_t* cmd, int argc, const char* const* argv, clic_res_t* clic_res)
{
    bool config_arg_present[CLIC_CONFIG_ARGC_MAX] = {false};
    bool user_arg_used[CLIC_USER_ARGC_MAX]        = {false};

    // First match the positional arguments
    // Assumption for now: positional arguments come first and they are all required
    for (uint8_t i = 0; i < cmd->argc; i++) {
        if (cmd->argv[i].type != CLIC_ARG_POSITIONAL) {
            break;
        }

        if (i >= argc) {
            return false;
        }

        clic_res->argv[i]     = (char*)argv[i];
        config_arg_present[i] = true;
        user_arg_used[i]      = true;
    }

    // Now match the arguments with values and the flags
    for (uint8_t i = 0; i < argc; i++) {
        if (user_arg_used[i]) {
            continue;
        }

        for (uint8_t j = 0; j < cmd->argc; j++) {
            if (config_arg_present[j]) {
                continue;
            }

            if (cmd->argv[j].type == CLIC_ARG_POSITIONAL) {
                continue;
            }

            for (const char* const* name = &cmd->argv[j].names[0]; *name != NULL; ++name) {
                if (strcmp(argv[i], *name) == 0) {
                    switch (cmd->argv[j].type) {
                        case CLIC_ARG_WITH_VALUE:
                            if (i + 1 < argc) {
                                clic_res->argv[j]     = (char*)argv[i + 1];
                                config_arg_present[j] = true;
                                user_arg_used[i]      = true;
                                user_arg_used[i + 1]  = true;
                            } else {
                                return false;
                            }
                            break;

                        case CLIC_ARG_FLAG:
                            clic_res->argv[j]     = "true";
                            config_arg_present[j] = true;
                            user_arg_used[i]      = true;
                            break;

                        default:
                            UNREACHABLE();
                    }
                }
            }
        }
    }

    // Check if all configured required args were present
    for (uint8_t i = 0; i < cmd->argc; i++) {
        if ((cmd->argv[i].required) && !config_arg_present[i]) {
            return false;
        }
    }

    // Check if all user provided args were used
    for (uint8_t i = 0; i < argc; i++) {
        if (!user_arg_used[i]) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Check if user configuration is valid
 *
 * TODO: More validations
 *
 * @param[in] cmdc -- Number of Commands
 * @param[in] cmdv -- Commands
 *
 * @return true  -- Configuration is valid
 * @return false -- Configuration is not valid
 */
static bool is_config_valid(uint8_t cmdc, const clic_cmd_t* cmdv)
{
    for (uint8_t i = 0; i < cmdc; i++) {
        if (cmdv[i].argc > CLIC_CONFIG_ARGC_MAX) {
            return false;
        }
    }

    return true;
}

static void version(void)
{
    printf(CLIC_VERSION "\n");
}

/**
 * @brief Print general help message
 *
 * @param[in] cmdc     -- Number of Commands
 * @param[in] cmdv     -- Commands
 * @param[in] is_error -- true: User did a wrong input, so we print an error message and the help message
 *                     -- false: User did nothing wrong, he just requested to print the help message
 */
static void help(uint8_t cmdc, const clic_cmd_t* cmdv, bool is_error)
{
    if (is_error) {
        printf(CLIC_USAGE_ERROR "\n\n");
    }

    printf("" CLIC_DESCRIPTION "\n"
           "\n"
           "\x1b[4mUsage:\x1b[24m " CLIC_NAME);

    if (cmdc > 0) {
        printf(" <COMMAND>\n"
               "\n"
               "\x1b[4mCommands:\x1b[24m\n");

        for (uint8_t i = 0; i < cmdc; i++) {
            printf("  %s     %s\n", cmdv[i].names[0], cmdv[i].description); // TODO: Add other names too
        }
    } else {
        printf("\n");
    }

    printf("\n"
           "\x1b[4mOptions:\x1b[24m\n"
           "  -h, --help     Print help\n"
           "  -V, --version  Print version\n");
}

static void help_cmd(const clic_cmd_t* cmd, bool is_error)
{
    if (is_error) {
        printf(CLIC_USAGE_ERROR "\n\n");
    }

    printf("%s\n"
           "\n"
           "\x1b[4mUsage:\x1b[24m " CLIC_NAME " %s",
           cmd->description,
           cmd->names[0]);

    for (uint8_t i = 0; i < cmd->argc; i++) {
        if (!cmd->argv[i].required) {
            continue;
        }

        switch (cmd->argv[i].type) {
            case CLIC_ARG_FLAG:
                printf(" %s", cmd->argv[i].names[0]);
                break;

            case CLIC_ARG_POSITIONAL:
                printf(" <%s>", cmd->argv[i].value_name);
                break;

            case CLIC_ARG_WITH_VALUE:
                printf(" %s <%s>", cmd->argv[i].names[0], cmd->argv[i].value_name);
                break;

            default:
                UNREACHABLE();
        }
    }

    printf(" [OPTIONS]\n");

    for (uint8_t i = 0; i < cmd->argc; i++) {
        if (cmd->argv[i].required) {
            printf("\n"
                   "\x1b[4mArguments:\x1b[24m\n"
                   "  ");
            break;
        }
    }

    for (uint8_t i = 0; i < cmd->argc; i++) {
        if (!cmd->argv[i].required) {
            continue;
        }

        switch (cmd->argv[i].type) {
            case CLIC_ARG_FLAG:
                // Nothing to do
                break;

            case CLIC_ARG_POSITIONAL:
                printf("<%s>  %s\n", cmd->argv[i].value_name, cmd->argv[i].description);
                break;

            case CLIC_ARG_WITH_VALUE:
                printf(" %s <%s>", cmd->argv[i].value_name, cmd->argv[i].value_name);
                break;

            default:
                UNREACHABLE();
        }
    }

    printf("\n"
           "\x1b[4mOptions:\x1b[24m\n");

    for (uint8_t i = 0; i < cmd->argc; i++) {
        if (cmd->argv[i].required) {
            continue;
        }

        switch (cmd->argv[i].type) {
            case CLIC_ARG_FLAG:
                printf("      %s  %s\n", cmd->argv[i].names[0], cmd->argv[i].description);
                break;

            case CLIC_ARG_POSITIONAL:
                printf("      <%s>  %s\n", cmd->argv[i].value_name, cmd->argv[i].description);
                break;

            case CLIC_ARG_WITH_VALUE:
                printf("      %s <%s>  %s\n", cmd->argv[i].names[0], cmd->argv[i].value_name, cmd->argv[i].description);
                break;

            default:
                UNREACHABLE();
        }
    }

    printf("  -h, --help        Print help\n");
}
