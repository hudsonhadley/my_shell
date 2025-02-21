#ifndef MY_FIRST_SHELL
#define MY_FIRST_SHELL

#include <stdbool.h>

void shell_loop();
char* read_line();
char** parse_line(char* line);
bool execute(char** args);
void deep_free(char** arr);

#endif // MY_FIRST_SHELL