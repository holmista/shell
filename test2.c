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

int main(int argc, char* argv[]){
    // char* res = insertSpacesAroundRedirection("ls tests/p2a-test>/tmp/output11");
    // printf("%s\n", res);
    char** args = parseCommand("ls tests/p2a-test>/tmp/output11");
    for(int i=0; args[i]!=NULL; i++){
        printf("%s\n", args[i]);
    }

    args = parseCommand("ls >a.txt");
    for(int i=0; args[i]!=NULL; i++){
        printf("%s\n", args[i]);
    }

    args = parseCommand("ls> a.txt");
    for(int i=0; args[i]!=NULL; i++){
        printf("%s\n", args[i]);
    }

    args = parseCommand("ls /folder1>a.txt");
    for(int i=0; args[i]!=NULL; i++){
        printf("%s\n", args[i]);
    }

    args = parseCommand("ls >");
    for(int i=0; args[i]!=NULL; i++){
        printf("%s\n", args[i]);
    }
}