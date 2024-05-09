#include "string.h"
#include <unistd.h>
#include <stdlib.h>

void printError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
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