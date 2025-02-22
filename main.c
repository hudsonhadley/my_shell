/**
 * @file main.c
 * @author Hudson Hadley
 * @date 2025-02-22
 * @brief Main function of a simple shell
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shell.h"

int main(int argc, char* argv[]) {
    // Load config files

    // Run command loop
    shell_loop();

    // Shutdown and cleanup


    return EXIT_SUCCESS;
}
