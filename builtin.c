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

bool mysh_exit(char** args) {
    return false;
}