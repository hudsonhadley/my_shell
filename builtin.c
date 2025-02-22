/**
 * @file builtin.c
 * @author Hudson Hadley
 * @date 2025-02-22
 * @brief Library of functions used for built in functions in the shell
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "builtin.h"

/**
 * Strings of function names
 */
char* built_in_str[] = {
    "cd",
    "exit",
    NULL
};

/**
 * Function calls in same order as in built_in_str
 */
bool (*built_in_func[]) (char**) = {
    &mysh_cd,
    &mysh_exit,
};

/**
 * @brief Performs "cd" on the command line. Note that this does not check if args[0] == "cd". It simply
 * assumes it and changes to the directory args[1].
 * @param args The list of arguments.
 * @return true if no errors occured worthy of breaking out of the shell (should be never)
 */
bool mysh_cd(char** args) {
    if (args == NULL || args[1] == NULL) {
        fprintf(stderr, "mysh: expected argument to \"cd\"\n");
        return true;
    }

    int status = chdir(args[1]);

    if (status != 0) {
        perror("msh");
    }

    return true;
}

/**
 * @brief Performs "exit" on the command line, altering that the shell should end.
 * @param args The arguments given. (N.B. Not used but so that all built-in's have similar signatures)
 * @return false
 */
bool mysh_exit(char** args) {
    return false;
}