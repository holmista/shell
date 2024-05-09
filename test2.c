#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <wait.h>

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

int main(int argc, char* argv[]){
    char** args = parseCommand("ls>a.txt");
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

    args = parseCommand("ls>a.txt");
    for(int i=0; args[i]!=NULL; i++){
        printf("%s\n", args[i]);
    }

    args = parseCommand("ls >");
    for(int i=0; args[i]!=NULL; i++){
        printf("%s\n", args[i]);
    }
}