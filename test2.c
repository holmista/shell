#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
    int kidpid;
    int fd = open("redirected.txt", O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) { perror("open"); abort(); }
    switch (kidpid = fork()) {
    case -1: perror("fork"); abort();
    case 0:
        char* args[] = {"ls", "-a", NULL};
        if (dup2(fd, 1) < 0) { perror("dup2"); abort(); }
        close(fd);
        execvp(args[0], args); perror("execvp"); abort();
    default:
        close(fd);
        /* do whatever the parent wants to do. */
    }

    switch (kidpid = fork()) {
    case -1: perror("fork"); abort();
    case 0:
        char* args[] = {"ls", "-a", NULL};
        execvp(args[0], args); perror("execvp"); abort();
    default:
        close(fd);
        /* do whatever the parent wants to do. */
    }
}