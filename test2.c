#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <wait.h>

void printError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

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


// Function to insert spaces around the '>' symbol if they do not already exist
char* insertSpacesAroundRedirection(char* command) {
    int len = strlen(command);
    int new_len = len;

    int pos = strchr(command, '>') - command;

    if (pos > 0 && command[pos - 1] != ' ') {
        new_len++;
    }
    
    if (pos < len - 1 && command[pos + 1] != ' ') {
        new_len++;
    }

    char* new_command = malloc(new_len + 1);  // +1 for null terminator
    if (!new_command) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    int i = 0, j = 0;

    while (i < len){
        if (i == pos){
            if (pos > 0 && command[pos - 1] != ' '){
                new_command[j++] = ' ';
            }
            new_command[j++] = command[i++];
            if (pos < len - 1 && command[pos + 1] != ' '){
                new_command[j++] = ' ';
            }
        } 
        else{
            new_command[j++] = command[i++];
        }
    }

    new_command[j] = '\0';
    return new_command;
}

char** parseCommand(char* command){
    char* trimmed = removeExcessWhitespaceFromBetween(command);

    if(strstr(trimmed, ">") != NULL){
        trimmed = insertSpacesAroundRedirection(trimmed);
    }

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
        printError();
        exit(1);
    }

    int argStartIdx = 0;
    int argumentCount = 0;


    for(int i=0; i<=len; i++){
        if(trimmed[i] == ' ' || trimmed[i] == '\0'){
            int argLen = i-argStartIdx+1;
            char* argument = (char*)malloc(argLen+1);
            if (argument == NULL) {
                // printf("failed to allocate memory for command argument");
                printError();
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
            if(bytesAmount <= 1){
                continue;
            }
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

char* concatenateStrings(char** strings) {
    if (strings == NULL) {
        return NULL;
    }

    int totalLength = 0;
    for (int i = 0; strings[i] != NULL; i++) {
        totalLength += strlen(strings[i]);
    }

    char* concatenated = malloc(totalLength + 1);
    if (concatenated == NULL) {
        printError();
        exit(1);
    }

    // Initialize the first position to the null character
    concatenated[0] = '\0';

    // Concatenate each string
    for (int i = 0; strings[i] != NULL; i++) {
        strcat(concatenated, strings[i]);
    }

    return concatenated;
}

int main(int argc, char* argv[]){
    // char* res = insertSpacesAroundRedirection("ls tests/p2a-test>/tmp/output11");
    // printf("%s\n", res);
    // char** args = parseInput("&\n");
    // printf("%i\n", doubleCharPointerLength(args));
    // for(int i=0; args[i]!=NULL; i++){
    //     printf("%s\n", args[i]);
    // }

    // args = parseCommand("ls >a.txt");
    // for(int i=0; args[i]!=NULL; i++){
    //     printf("%s\n", args[i]);
    // }

    // args = parseCommand("ls> a.txt");
    // for(int i=0; args[i]!=NULL; i++){
    //     printf("%s\n", args[i]);
    // }

    // args = parseCommand("ls /folder1>a.txt");
    // for(int i=0; args[i]!=NULL; i++){
    //     printf("%s\n", args[i]);
    // }

    // args = parseCommand("ls >");
    // for(int i=0; args[i]!=NULL; i++){
    //     printf("%s\n", args[i]);
    // }

    char* a = "a";
    char* b = "b";

    char** c = malloc(16);
    c[0] = a;
    c[1] = b;

    char* res = concatenateStrings(c);
    printf("%s\n", res);
}