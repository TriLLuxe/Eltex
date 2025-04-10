#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "header.h"

int main() {
    char input[256];
    char *args[10];
    char program_path[256];
    while (1) {
        printf("interpreter> ");
        

        if (fgets(input, 256, stdin) == NULL) {
            printf("\nВыход из программы\n");
            return 0;
        }

        if (input[0] == '\n') {
            continue;
        }

        int num_args = 0;
        char *token = strtok(input, " \n");
        while (token != NULL && num_args < 9) {
            args[num_args] = token;
            num_args++;
            token = strtok(NULL, " \n");
        }
        args[num_args] = NULL;

        if (strcmp(args[0], "exit" ) == 0|| strcmp(args[0], "q" ) == 0) {
            printf("Выход из программы\n");
            return 0;
        }

        strcpy(program_path, "./");
        strcat(program_path, args[0]);
        args[0] = program_path;

        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Ошибка создания процесса\n");
            continue;
        } 
        else if (pid == 0) {
            execute_program(args[0], args);
            fprintf(stderr, "Ошибка запуска программы %s\n", args[0]);
            exit(1);
        } 
        else {
            wait(NULL);
        }
    }
    
    return 0;
}