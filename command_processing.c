#include "unistd.h"
#include <stdio.h>
#include <wait.h>
#include <string.h>
#include <stdlib.h>

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
        printf("failed to allocate memory for commands");
        exit(1);
    }

    int commandStartIdx = 0;
    int commandCount = 0;

    for (int i = commandStartIdx; i <= len; i++) {
        if (input[i] == '&' || input[i] == '\n') {
            int bytesAmount = i - commandStartIdx;
            char* command = (char*)malloc(bytesAmount + 1);  // add +1 for null terminator
            if (command == NULL) {
                printf("failed to allocate memory for command");
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
        printf("failed to allocate memory for arguments");
        exit(1);
    }

    int argStartIdx = 0;
    int argumentCount = 0;


    for(int i=0; i<=len; i++){
        if(trimmed[i] == ' ' || trimmed[i] == '\0'){
            int argLen = i-argStartIdx+1;
            char* argument = (char*)malloc(argLen+1);
            if (argument == NULL) {
                printf("failed to allocate memory for command argument");
                exit(1);
            }

            int j = 0;
            for (int k = argStartIdx; k < i; k++) {
                argument[j] = trimmed[k];
                j++;
            }
            argument[j] = '\0';

            argStartIdx = i + 1;
            argumentPointers[argumentCount] = argument;
            argumentCount++;
        }
    }

    argumentPointers[argumentCount] = NULL;
    return argumentPointers;
}