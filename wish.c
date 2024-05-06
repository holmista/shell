#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <fcntl.h>
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

    char* paths[256];
    int pathsIdx = 0;

    while(1){
        printf("wish> ");
        char* lineptr = NULL;
        size_t len = 0;
        ssize_t res =  getline(&lineptr, &len, stdin);
        if(res == -1){
            printf("failed to read inputted line\n");
            continue;
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
            char** arguments = parseCommand(commands[i]);
            int argumentsLength = length(arguments);

            // handle cd
            if(strcmp(arguments[0], "cd") == 0){
                if(argumentsLength != 2){
                    printf("cd can only have one argument\n");
                    freeDoubleCharPointer(arguments);
                    continue;
                }
                int res = chdir(arguments[1]);
                if(res == -1){
                    perror("cd");
                    freeDoubleCharPointer(arguments);
                    continue;
                }
                freeDoubleCharPointer(arguments);
                continue;
            }

            // handle exit
            else if(strcmp(arguments[0], "exit") == 0){
                printf("exiting...\n");
                freeDoubleCharPointer(arguments);
                exit(0);
            }

            // handle path
            else if(strcmp(arguments[0], "path") == 0){
                for (int i = 0; arguments[i] != NULL; i++) {
                    paths[i] = NULL;
                }
                pathsIdx = 0;

                int i;
                for(i=1; i<argumentsLength; i++){
                    paths[pathsIdx] = strdup(arguments[i]);  // create a copy of the path
                    pathsIdx++;
                }
                paths[pathsIdx] = NULL;

                freeDoubleCharPointer(arguments);
                continue;
            }

            freeDoubleCharPointer(arguments);

            int pid = fork();
            if(pid < 0){
                printf("failed to create process for the command execution\n");
                i++;
                continue;
            }
            if(pid == 0){
                char** arguments = parseCommand(commands[i]);
                int redirectionIdx = commandContainsRedirection(arguments);
                int argumentsLength = length(arguments);

                // replace command with it's full path
                if (strstr(arguments[0], "/") == NULL){ // is a command like ls
                    char* commandFullPath = getCommandFullPath(paths, arguments[0]);
                    if(commandFullPath == NULL){
                        printf("could not execute command: %s\n", arguments[0]);
                        freeDoubleCharPointer(arguments);
                        exit(0);
                    }
                    arguments[0] = commandFullPath;
                }

                // handle redirection
                if(redirectionIdx > -1){
                    char** validArguments = malloc((int)sizeof(char*) * argumentsLength); // this is an overkill but won't hurt
                    int k;
                    for(k=0; k<redirectionIdx; k++){
                        validArguments[k] = arguments[k];
                    }
                    validArguments[k] = NULL;
                    char* fileName = arguments[redirectionIdx+1];

                    int fd = open(fileName, O_WRONLY|O_TRUNC|O_CREAT, 0644);
                    if (fd < 0){
                        printf("could not open file for redirection\n");
                        freeDoubleCharPointer(validArguments);
                        exit(0);
                    }
                    if (dup2(fd, STDOUT_FILENO) < 0){ 
                        printf("could not redirect stdout to a specified file\n");
                        freeDoubleCharPointer(validArguments);
                        exit(0);
                    }
                    close(fd);

                    int res = execv(validArguments[0], validArguments);
                    if (res == -1){
                        printf("could not execute the command\n");
                    }
                    freeDoubleCharPointer(validArguments);
                }

                else{
                    int res = execv(arguments[0], arguments);
                    if (res == -1){
                        printf("could not execute the command\n");
                    }
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