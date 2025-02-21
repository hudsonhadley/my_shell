#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shell.h"

/**
 * @brief Executes the shell loop until the user exits. Each loop has three steps: read, parse, and
 * execute.
 */
void shell_loop() {

    while (true) {
        printf("> ");

        char* line = read_line();

        // If an error occurred in read_line()
        if (line == NULL) {
            break;
        }

        char** args = parse_line(line);

        if (args == NULL) {
            break;
        }

        int idx = 0;
        printf("[ ");
        while (args[idx] != NULL) {
            printf("'%s' ", args[idx]);
            idx++;
        }
        printf("]\n");

        int status = execute(args);

        free(line);
        deep_free(args); // We have to free each individual string too

        if (!status) {
            break;
        }
    }
}

/**
 * @brief Reads a line from the terminal.
 * @return A string from the terminal
 */
char* read_line() {
    int const BUFF_ADD = 50;


    int buffsize = BUFF_ADD;
    int position = 0;
    char* buffer = (char*) calloc(sizeof(char), buffsize);

    if (buffer == NULL) {
        fprintf(stderr, "my_shell: allocation error\n");
        return NULL;
    }

    // Will end when EOF or \n is read, or if an error occurs
    while (true) {
        char c = (char) getchar();

        // End of line
        if (c == EOF || c == '\n') {
            buffer[position] == '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;


        // If we exceed the buffer size, we need to reallocate to make space
        if (position >= buffsize) {
            buffsize += BUFF_ADD;
            char* new_buffer = (char*) calloc(sizeof(char), buffsize);

            // Copy old elements over
            for (int i = 0; i < buffsize - BUFF_ADD; i++) {
                new_buffer[i] = buffer[i];
            }

            free(buffer);
            buffer = new_buffer;

            if (buffer == NULL) {
                fprintf(stderr, "my_shell: allocation error\n");
                return NULL;
            }
        }
    }
    return NULL;
}

/**
 * @brief Parses a line from the command line. We will assume the first word is an executable.
 * Following this will be arguments for that command. This also assumes that the input is terminated
 * with a '\0'.
 * @param line The string to parse
 * @return A parsed array of strings terminated by a NULL pointer
 */
char** parse_line(char* line) {
    size_t SIZE_ADD = 5; // We'll add in increments of 5 if we need to grow
    
    size_t SIZE = SIZE_ADD;
    char** args = (char**) malloc(sizeof(char*) * SIZE);
    size_t curr = 0;
    size_t idx = 0;

    while (line[idx] == ' ') {
        idx++;
    }

    while (line[idx] != '\0') {
        // Find the end of the current word
        size_t end = idx;
        while (line[end] != ' ' && line[end] != '\0') {
            end++;
        }

        size_t len = end - idx;
        char* str = (char*) calloc(sizeof(char), len + 1);

        // Copy it over
        for (size_t i = 0; i < len; i++) {
            str[i] = line[idx + i];
        }
        str[len] = '\0';

        idx = end;
        while (line[idx] == ' ') {
            idx++;
        }

        // Regrow if need be
        if (curr >= SIZE - 1) {
            SIZE += SIZE_ADD;
            
            char** new_args = (char**) malloc(sizeof(char*) * SIZE);

            for (int i = 0; i < SIZE - SIZE_ADD; i++) {
                new_args[i] = args[i];
            }
            free(args);
            args = new_args;
        }


        args[curr] = str;
        curr++;
    }

    args[curr] = NULL;

    return args;
}

/**
 * @brief Executes a parsed command. This assumes that the first element in the array is a command,
 * and is followed by a list of arguments. It also assumes that this array is terminated by the NULL
 * pointer.
 * @param args A NULL terminated array of strings
 * @return The status of the execution. true indicates the user wants to continue. false indicates
 * the user wants to leave the shell.
 */
bool execute(char** args) {
    return true;
}


/**
 * @brief Safely frees all the memory allocated by a two dimension char array, or a one dimensional
 * array of strings. The free() function is called on each element, and then on the pointer itself. 
 * It should be noted that this function assumes the array is terminated by a NULL pointer.
 * @param arr The 2D array
 */
void deep_free(char** arr) {
    // Don't do anything if it is a NULL pointer passed in
    if (arr == NULL) {
        return;
    }

    int idx = 0;

    while (arr[idx] != NULL) {
        free(arr[idx]);
        idx++;
    }

    free(arr);
}