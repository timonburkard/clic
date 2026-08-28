# `>_` CLIC

CLIC is a small command-line interface parser for C. Applications describe their commands and arguments with static data, then pass that configuration to `clic_parse()`.

## Features

- Commands with aliases and callbacks
- Positional arguments
- Flags and options with values
- Built-in command help
- Built-in version output
- Compile-time configuration
- No third-party runtime dependencies

## Basic usage

An application defines commands and their arguments. Name arrays must end with `NULL`.

### Example

```c
enum {
    ARG_ID_NAME = 0,
    ARG_ID_NUMBER,
};

static const char* const cmd_hello_names[]  = {"hello", NULL};
static const char* const cmd_bye_names[]    = {"bye", NULL};
static const char* const arg_number_names[] = {"-n", "--number", NULL};

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
        .names       = arg_number_names,
        .value_name  = "N",
        .description = "Number of greetings",
    },
};

static const clic_cmd_t commands[] = {
    {
        .names       = cmd_hello_names,
        .description = "Say hello",
        .function    = hello,
        .argc        = (uint8_t)(sizeof(arguments) / sizeof(arguments[0])),
        .argv        = arguments,
    },
    {
        .names       = cmd_bye_names,
        .description = "Say bye",
        .function    = bye,
        .argc        = (uint8_t)(sizeof(arguments) / sizeof(arguments[0])),
        .argv        = arguments,
    },
};

int main(int argc, char** argv)
{
    return (int)clic_parse(
        (uint8_t)(sizeof(commands) / sizeof(commands[0])),
        commands,
        argc,
        (const char* const*)argv
    );
}
```

The callback function receives a `clic_res_t`. Its `argv` entries use the same indices as the configured argument array. An entry is `NULL` when its optional argument was not provided.

- See [`test/greet.c`](test/greet.c) for an example application using two commands.
- See [todo.c](https://github.com/timonburkard/todo.c) for a real application using multiple commands.

#### Help

Help output for the example program:

```sh
> greet --help
Simple program to greet someone

Usage: greet <COMMAND>

Commands:
  hello   Say hello
  bye     Say goodbye

Options:
  -h, --help     Print help
  -V, --version  Print version
```

Help output for a specific command of the example program:

```sh
> greet hello --help
Say hello

Usage: greet hello <NAME> [OPTIONS]

Arguments:
  <NAME>  Name to greet

Options:
  -n <N>      Number of greetings
  -h, --help  Print help

```

## Argument types

| Type                  | Example       | Description                        |
| --------------------- | ------------- | ---------------------------------- |
| `CLIC_ARG_POSITIONAL` | `hello Alice` | A positional value such as `Alice` |
| `CLIC_ARG_WITH_VALUE` | `--number 3`  | A named option followed by a value |
| `CLIC_ARG_FLAG`       | `--verbose`   | A named switch with no value       |

When a flag is present, its result entry contains the string `"true"`.

## Configuration

Every setting in [`src/clic_config.h`](src/clic_config.h) has a default and can be overridden with a compiler definition.

| Setting                   | Default                | Purpose                                      |
| ------------------------- | ---------------------- | -------------------------------------------- |
| `CLIC_NAME`               | `"name"`               | Application name shown in help               |
| `CLIC_DESCRIPTION`        | `"Description"`        | Application description shown in help        |
| `CLIC_VERSION`            | `"v1.0.0"`             | Text printed for the version option          |
| `CLIC_CONFIG_ARGC_MAX`    | `10`                   | Maximum arguments configured for one command |
| `CLIC_USER_ARGC_MAX`      | `20`                   | Maximum arguments accepted from the user     |
| `CLIC_USAGE_ERROR`        | Colored error text     | Heading printed before help on invalid input |
| `CLIC_VERSION_NAMES(arg)` | `--version`, `-V`      | Predicate identifying version options        |
| `CLIC_HELP_NAMES(arg)`    | `--help`, `-h`, `help` | Predicate identifying help options           |

## Build the example

```sh
./build.sh
```

Then run the example:

```sh
$ ./greet hello Alice --number 2
Hello, Alice!
Hello, Alice!
```

## Tests

The project uses CTest for black-box CLI tests covering output, exit codes, aliases, help, version, and invalid input.

```sh
ctest --test-dir build --output-on-failure
```

## Current limitations

- Positional arguments must appear first and are always required.
- Help output displays the first configured name for each command or option.
