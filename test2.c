#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]){
    char* command = "ls";
    char* d = "/bin";
    char fullPath[1024]; // it's an overkill but I don't want to debug another memory issue
    
    strcpy(fullPath, d);
    strcat(fullPath, "/");
    strcat(fullPath, command);

    printf("%s\n", fullPath);
    return 0;
}