#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include "command_processing.h"

int main(int argc){
    if(argc > 1){
        printf("can receive only 1 optional argument\n");
        return 1;
    }
    // lets assume it's an interactive shell at first
    // print wish> and run a while loop until the read line is exit or EOF
    // the way commands will be received is that user will type somethng and upon pressing enter I'll get the command they typed
    // I'll then process the command
    // go back to while loop line
    while(1){
        printf("wish> ");
        char* lineptr = NULL;
        size_t len = 0;
        ssize_t res =  getline(&lineptr, &len, stdin);
        if(res == -1){
            printf("failed to read inputted line\n");
            continue;
        }

        if(strcmp(lineptr, "exit\n") == 0){
            printf("exiting...\n");
            exit(0);
        }

        if(strcmp(lineptr, "\n") == 0){
            continue;
        }

        char** commands = parseInput(lineptr);
        // for each command a child process should be created, in parallel
        int i;
        int commandsAmount = doubleCharPointerLength(commands);
        int pids[commandsAmount];

        for (i=0; i<commandsAmount; i++)
        {
            int pid = fork();
            if(pid < 0){
                printf("failed to execute the command\n");
                i++;
                continue;
            }
            if(pid == 0){
                char** arguments = parseCommand(commands[i]);
                int res = execvp(arguments[0], arguments);
                printf("errorno: %i\n", errno);
                if (res == -1){
                    printf("could not execute the command\n");
                }
                freeDoubleCharPointer(arguments);
                exit(0);
            }
            else{
                pids[i] = pid;
            }
        }

        // wait for all child processes to complete
        for (int i = 0; i < commandsAmount; i++) {
            waitpid(pids[i], NULL, 0);
        }
        
        // free memory
        free(lineptr);
        freeDoubleCharPointer(commands);
    }
    return 0;
}