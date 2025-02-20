#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


void shell_loop();
char* read_line();
char** parse_line(char* line);
bool execute(char** args);

void deep_free(char** arr);

int main(int argc, char* argv[]) {
    // Load config files

    // Run command loop
    shell_loop();

    // Shutdown and cleanup


    return EXIT_SUCCESS;
}

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
        printf("You typed: %s\n", line);

        char** args = parse_line(line);
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
    int const BUFF_ADD = 1024;


    int buffsize = BUFF_ADD;
    int position = 0;
    char* buffer = (char*) malloc(sizeof(char) * buffsize);

    if (buffer == NULL) {
        fprintf(stderr, "my_sh: allocation error\n");
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
            buffer = (char*) realloc(buffer, buffsize);

            if (buffer == NULL) {
                fprintf(stderr, "my_sh: allocation error\n");
                return NULL;
            }
        }
    }
    return NULL;
}

/**
 * @brief Parses a line from the command line. We will assume the first word is an executable.
 * Following this will be arguments for that command.
 * @param line The string to parse
 * @return A parsed array of strings terminated by a NULL pointer
 */
char** parse_line(char* line) {
    return NULL;
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