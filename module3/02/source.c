#include <unistd.h>
#include "header.h"

void execute_program(char *program, char *args[]) {
    
    if (execv(program, args) == -1) {
        return;
    }
}