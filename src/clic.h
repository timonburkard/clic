#ifndef CLI_H
#define CLI_H

#include <stdbool.h>
#include <stdint.h>

#include "clic_config.h"

typedef enum {
    CLIC_ERR_OK      = 0,    // No error :-)
    CLIC_ERR_ARG     = 1,    // Invalid arguments choosen by the user
    CLIC_ERR_CONFIG  = 2,    // Invalid configuration (uint8_t cmdc, const clic_cmd_t* cmdv) was provided
    CLIC_ERR_NULL    = 3,    // Null pointer was provided
    CLIC_ERR_GENERAL = 0xFF, // General/unspecific error code
} clic_err_t;

typedef enum {
    CLIC_ARG_FLAG,       // E.g., `ls -l`, `ls --long`
    CLIC_ARG_WITH_VALUE, // E.g., `ls --block-size K`
    CLIC_ARG_POSITIONAL, // E.g., `ls ~/documents`
} clic_arg_type_t;

typedef struct {
    char* argv[CLIC_CONFIG_ARGC_MAX];
} clic_res_t;

typedef struct {
    clic_arg_type_t    type;
    bool               required;
    const char* const* names;      // Only used for FLAG and WITH_VALUE
    const char*        value_name; // Only used for WITH_VALUE and POSITIONAL
    const char*        description;
} clic_arg_t;

typedef struct {
    const char* const* names;
    const char*        description;
    clic_err_t (*function)(clic_res_t*);
    uint8_t           argc;
    const clic_arg_t* argv;
} clic_cmd_t;

/**
 * @brief CLIC parse
 *
 * @param[in] cmdc     -- Number of Commands
 * @param[in] cmdv     -- Commands
 * @param[in] argc     -- Number of user arguments, from main()
 * @param[in] argv     -- User arguments, from main()
 *
 * @return clic_error_t -- Error code
 */
clic_err_t clic_parse(uint8_t cmdc, const clic_cmd_t* cmdv, int argc, const char* const* argv);

#endif // CLI_H
