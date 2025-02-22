#ifndef MY_FIRST_SHELL_BUILTIN
#define MY_FIRST_SHELL_BUILTIN

#include <stdbool.h>
#include <stdlib.h>

extern char* built_in_str[];
extern bool (*built_in_func[]) (char**);


bool mysh_cd(char** args);
bool mysh_exit(char** args);


#endif // MY_FIRST_SHELL_BUILTIN