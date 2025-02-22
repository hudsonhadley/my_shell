/**
 * @file shell.c
 * @author Hudson Hadley
 * @date 2025-02-22
 * @brief Library of functions used on the shell for looping and execution
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "shell.h"
#include "builtin.h"

/************************************************************************************************
 * HELPER FUNCTIONS
 ***********************************************************************************************/
bool launch(char** args);
bool is_whitespace(char c);
int find_match(char* s, int idx);

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

        // Error in parsing (probably unclosed quotation)
        if (args == NULL) {
            fprintf(stderr, "mysh: error parsing (probably unclosed quotation mark)\n");
            continue;
        }

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
        fprintf(stderr, "mysh: allocation error\n");
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
                fprintf(stderr, "mysh: allocation error\n");
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

    // Find the first non whitespace character
    while (line[idx] != '\0' && is_whitespace(line[idx])) {
        idx++;
    }

    while (line[idx] != '\0') {

        // Find the end of the current word
        size_t end = idx;
        int quote_count = 0; // These will be omitted

        while (!is_whitespace(line[end]) && line[end] != '\0') {

            // If we have a quote, find the end of it
            if (line[end] == '\'' || line[idx] == '\"') {
                int match_idx = find_match(line, end);

                if (match_idx < 0) {
                    return NULL;
                }

                quote_count += 2;
                end += match_idx;

            } else {

                end++;
            }
        }

        size_t len = end - idx;
        len -= quote_count;

        char* str = (char*) calloc(sizeof(char), len + 1);

        // Copy it over
        int line_idx = idx;
        for (size_t i = 0; i < len; i++) {
            // Skip quotes
            while (line[line_idx] == '\'' || line[line_idx] == '\"') {
                line_idx++;
            }
            str[i] = line[line_idx];
            line_idx++;
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
 * @brief Finds a matching character after the current idx
 * @param s The string
 * @param idx the index of the character to match
 * @return the index of the matching character, or -1 if no character found
 */
int find_match(char* s, int idx) {
    int match_idx = idx + 1;

    while (s[match_idx] != '\0') {
        if (s[match_idx] == s[idx]) {
            return match_idx;
        }

        match_idx++;
    }

    return -1; // If none found
}

/**
 * @brief Tells whether or not a character is a whitespace character
 * @param c the character to check
 * @return true if the character is whitespace
 */
bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
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
    if (args == NULL) {
        return false;
    } else if (args[0] == NULL) {
        return true;
    }

    int idx = 0;
    while (built_in_str[idx] != NULL) {
        if (strcmp(args[0], built_in_str[idx]) == 0) {
            return (built_in_func[idx]) (args);
        }
        idx++;
    }

    return launch(args);
}

/**
 * @brief Launches a parsed command assuming that it is a NULL terminated array of strings.
 * @param args The list of arguments
 * @return true if no errors occurred
 */
bool launch(char** args) {
    if (args == NULL) { // Ideally this should never occur, but just a safeguard
        return false;
    }

    pid_t pid = fork(); // Splits the process

    // Child process --> run the new command
    if (pid == 0) {
        int return_value = execvp(args[0], args); // If nothing goes wrong, nothing should be returned
        
        // Only returns
        if (return_value == -1) {
            perror("mysh");
        }
        exit(EXIT_FAILURE); // Only will reach if execvp has error and returns

    // Error forking the process
    } else if (pid == -1) {
        perror("mysh");

    // Parent process -> wait for the child to end
    } else {
        int status;

        while (true) {
            pid_t w_pid = waitpid(pid, &status, WUNTRACED); // Return if child has stopped

            // If we exit normally or if we exitted with a signal, break
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                break;
            }
        }
    }

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