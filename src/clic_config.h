#ifndef CLIC_CONFIG_H
#define CLIC_CONFIG_H

#include <string.h>

#ifndef CLIC_NAME
#define CLIC_NAME "name"
#endif

#ifndef CLIC_DESCRIPTION
#define CLIC_DESCRIPTION "Description"
#endif

#ifndef CLIC_VERSION
#define CLIC_VERSION "v1.0.0"
#endif

#ifndef CLIC_CONFIG_ARGC_MAX
#define CLIC_CONFIG_ARGC_MAX 10
#endif

#ifndef CLIC_USER_ARGC_MAX
#define CLIC_USER_ARGC_MAX 20
#endif

#ifndef CLIC_USAGE_ERROR
#define CLIC_USAGE_ERROR "\x1b[31mError\x1b[39m, this is how to use:"
#endif

#ifndef CLIC_VERSION_NAMES
#define CLIC_VERSION_NAMES(arg) ((strcmp((arg), "--version") == 0) || (strcmp((arg), "-V") == 0))
#endif

#ifndef CLIC_HELP_NAMES
#define CLIC_HELP_NAMES(arg) ((strcmp((arg), "--help") == 0) || (strcmp((arg), "-h") == 0) || (strcmp((arg), "help") == 0))
#endif

#endif // CLIC_CONFIG_H
