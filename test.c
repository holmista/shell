#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <fcntl.h>

void printError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

// assumes it will be NULL terminated
int length(char** ptr){
    int count = 0;
    while (*ptr != NULL) { 
        count++;
        ptr++;
    }
    return count;
}

// assumes it will be NULL terminated
void freeDoubleCharPointer(char** ptr){
    int i = 0;
    while(ptr[i] != NULL){
        free(ptr[i]);
        i++;
    }
    free(ptr);
}

// assumes it will be NULL terminated
// NULL is not counted
int doubleCharPointerLength(char** ptr){
    int i = 0;
    while(ptr[i] != NULL){
        i++;
    }
    return i;
}

char* trimLeading(char* string) {
    int len = strlen(string);
    char* trimmed = malloc(len + 1);  // add +1 for null terminator
    
    int i;
    for (i = 0; string[i] == ' '; i++);  // find the index of the first non-space character
    
    strcpy(trimmed, string + i);  // copy the trimmed string
    
    return trimmed;
}

char* trimTrailing(char* string) {
    int len = strlen(string);
    char* trimmed = malloc(len + 1);  // add +1 for null terminator
    
    int i;
    for (i = len - 1; i >= 0 && string[i] == ' '; i--);  // find the index of the last non-space character
    
    strncpy(trimmed, string, i + 1);  // copy the trimmed string
    trimmed[i + 1] = '\0';  // add null terminator at the end of the trimmed string
    
    return trimmed;
}

char* trimLeadingAndTrailing(char* string){
    char* trimmedLeading = trimLeading(string);
    char* trimmed = trimTrailing(trimmedLeading);
    free(trimmedLeading);
    return trimmed;
}

/*
assumes the string is trimmed leading and trailing
leaves 1 space between characters, removes if there are more
*/
char* removeExcessWhitespaceFromBetween(char* string){
    int len = strlen(string);
    char* trimmed = malloc(len + 1);  // add +1 for null terminator

    int i, j;
    int isSpace = 0;

    for (i = 0, j = 0; i < len; i++) {
        if (string[i] != ' ') {
            trimmed[j] = string[i];
            isSpace = 0;
            j++;
        } else if (!isSpace) {
            trimmed[j++] = ' ';
            isSpace = 1;
        }
    }

    trimmed[j] = '\0';  // add null terminator at the end of the trimmed string
    return trimmed;
}

/*
breaks up inputted line containing one or more commands into seperate commands, trims and returns them.
e.g ls -a && cd ../ & ./program -> ["ls -a", "cd ../", "./program"]
it is caller's responsibility to free each individual command and the array of commands
    for (int i = 0; i < commandCount; i++) {
        free(commandPointers[i]);
    }
    free(commandPointers);
*/

char** parseInput(char* input){
    // char* a = "ls\n";
    int len = strlen(input);
    int commandsAmount = 1;

    // iterate over the line and determine how many commands are there
    for (int i = 0; input[i] != '\n'; i++) {
        if (input[i] == '&') {
            commandsAmount++;
        }
    }

    // separate command into separate commands
    char** commandPointers = (char**)malloc((commandsAmount + 1) * sizeof(char*)); // +1 for NULL at the end
    if (commandPointers == NULL) {
        // printf("failed to allocate memory for commands");
        printError();
        exit(1);
    }

    int commandStartIdx = 0;
    int commandCount = 0;

    for (int i = commandStartIdx; i <= len; i++){
        if (input[i] == '&' || input[i] == '\n'){
            int bytesAmount = i - commandStartIdx;
            char* command = (char*)malloc(bytesAmount + 1);  // add +1 for null terminator
            if (command == NULL) {
                // printf("failed to allocate memory for command");
                printError();
                exit(1);
            }

            int j = 0;
            for (int k = commandStartIdx; k < i; k++) {
                command[j] = input[k];
                j++;
            }
            command[j] = '\0';  // add null terminator at the end of the command

            commandStartIdx = i + 1;
            char* trimmedCommand = trimLeadingAndTrailing(command);
            free(command);
            commandPointers[commandCount] = trimmedCommand;
            commandCount++;
        }
    }

    commandPointers[commandCount] = NULL;
    return commandPointers;
}

/*
breaks up command by it's componentsand returns them.
e.g. "ls   -a" -> ["ls", "-a"]
1. the command should be trimmed
2. the command will be parsed no matter the whitespace between arguments. e.g. "ls    -a" is valid and when parsed the excess whitespace won't matter  
3. the delimiter is assumed to be whitespace between commands, this can be either one or more spaces.

it is caller's responsibility to free each individual argument and the array of arguments
*/
char** parseCommand(char* command){
    char* trimmed = removeExcessWhitespaceFromBetween(command);

    // get the amount of arguments
    int len = strlen(trimmed);
    int argumentsAmount = 1;
    for(int i=0; i<len; i++){
        if(trimmed[i] == ' '){
            argumentsAmount++;
        }
    }

    char** argumentPointers = malloc((int)sizeof(char*)*(argumentsAmount+1));
    if (argumentPointers == NULL) {
        // printf("failed to allocate memory for arguments");
        // printError();
        exit(1);
    }

    int argStartIdx = 0;
    int argumentCount = 0;


    for(int i=0; i<=len; i++){
        if(trimmed[i] == ' ' || trimmed[i] == '\0' || trimmed[i] == '>'){
            int argLen = i-argStartIdx+1;
            char* argument = (char*)malloc(argLen+1);
            if (argument == NULL) {
                // printf("failed to allocate memory for command argument");
                // printError();
                exit(1);
            }

            int j = 0;
            for (int k = argStartIdx; k < i; k++) {
                argument[j] = trimmed[k];
                j++;
            }
            argument[j] = '\0';

            // printf("%s\n", argument);

            if(strcmp(argument, "") != 0){
                argStartIdx = i + 1;
                argumentPointers[argumentCount] = argument;
                argumentCount++;
            }

            if(trimmed[i] == '>'){
                char* argument = (char*)malloc(2);
                if (argument == NULL) {
                    // printf("failed to allocate memory for command argument");
                    // printError();
                    exit(1);
                }
                argument[0] = '>';
                argument[1] = '\0';

                int inc;
                if(i+1<len){
                    if(trimmed[i+1] == ' '){
                        inc = 2;
                    }
                    else inc = 1;
                }

                argStartIdx = i + inc;
                argumentPointers[argumentCount] = argument;
                argumentCount++;
            }
        }
    }

    argumentPointers[argumentCount] = NULL;
    return argumentPointers;
}

/*
this should get parsed command as an input from parseCommand function
if a command contains redirection symbol > then it returns it's index, else -1
*/ 
int commandContainsRedirection(char** command){
    for(int i=0; command[i]!=NULL; i++){
        if(strcmp(command[i], ">") == 0){
            return i;
        } 
    }
    return -1;
}

/*
receives full path of directories and the name of executable. 
e.g. ["/bin", "/bin/usr"], "ls"
returns full path of the command, if the command was not found returns NULL
*/
char* getCommandFullPath(char** directories, char* command){
    if(directories == NULL){
        return NULL;
    }

    int i = 0;
    while(directories[i] != NULL){
        char fullPath[1024]; // it's an overkill but I don't want to debug another memory issue
        strcpy(fullPath, directories[i]);
        strcat(fullPath, "/");
        strcat(fullPath, command);

        if (access(fullPath, X_OK) == 0) {
            return strdup(fullPath);
        }

        i++;
    }
    return NULL;
}



#define INTERACTIVE_MODE 0
#define BATH_MODE 1

int main(int argc, char* argv[]){
    int mode = INTERACTIVE_MODE;
    FILE* in = stdin;
    if(argc == 2){
        mode = BATH_MODE;
        in = fopen(argv[1], "r");
        if(in == NULL){
            exit(1);
        }
    }
    if(argc > 2){
      printError();
      exit(1);
    }

    char* paths[256] = {"/bin"};
    int pathsIdx = 0;

    while(1){
        if(mode == INTERACTIVE_MODE){
            printf("wish> ");
        }
        char* lineptr = NULL;
        size_t len = 0;
        ssize_t res =  getline(&lineptr, &len, in);
        if(res == -1){
            // printf("failed to read inputted line\n");
            printError();
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
                    // printf("cd can only have one argument\n");
                    printError();
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
            else if(strcmp(arguments[0], "exit") == 0 && argumentsLength == 1){
                // printf("exiting...\n");
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

            // freeDoubleCharPointer(arguments);

            else {
                arguments = parseCommand(commands[i]);
                printf("here1");
                for(int i=0; arguments[i] != NULL; i++){
                    printf("%s\n", arguments[i]);
                }
                int redirectionIdx = commandContainsRedirection(arguments);
                argumentsLength = length(arguments);

                // replace command with it's full path
                if (strstr(arguments[0], "/") == NULL){ // is a command like ls
                    char* commandFullPath = getCommandFullPath(paths, arguments[0]);
                    if(commandFullPath == NULL){
                        // printf("could not execute command: %s\n", arguments[0]);
                        printError();
                        freeDoubleCharPointer(arguments);
                        exit(0);
                    }
                    arguments[0] = commandFullPath;
                }

                printf("%i\n", redirectionIdx);
                // handle redirection
                if(redirectionIdx > -1){
                    char** validArguments = malloc((int)sizeof(char*) * argumentsLength); // this is an overkill but won't hurt
                    int k;
                    for(k=0; k<redirectionIdx; k++){
                        validArguments[k] = arguments[k];
                    }
                    validArguments[k] = NULL;
                    int validArgumentsLength = length(validArguments);

                    printf("%i\n", argumentsLength - validArgumentsLength);
                    // there should only be 1 argument after >
                    if(argumentsLength - validArgumentsLength != 2){
                        printf("here");
                        printError();
                        // freeDoubleCharPointer(arguments);
                        exit(0);
                    }

                    char* fileName = arguments[redirectionIdx+1];

                    int fd = open(fileName, O_WRONLY|O_TRUNC|O_CREAT, 0644);
                    if (fd < 0){
                        // printf("could not open file for redirection\n");
                        printError();
                        freeDoubleCharPointer(validArguments);
                        exit(0);
                    }
                    if (dup2(fd, STDOUT_FILENO) < 0){ 
                        // printf("could not redirect stdout to a specified file\n");
                        printError();
                        freeDoubleCharPointer(validArguments);
                        exit(0);
                    }
                    close(fd);

                    int res = execv(validArguments[0], validArguments);
                    // fprintf(stdout, res);
                    if (res == -1){
                        printError();
                        // printf("could not execute the command\n");
                    }
                    freeDoubleCharPointer(validArguments);
                }

                else{
                    int res = execv(arguments[0], arguments);
                    // printf("here");
                    if (res == -1){
                        // printf("could not execute the command\n");
                        printError();
                    }
                }
            }
        }
    }
    return 0;
}