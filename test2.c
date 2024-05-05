#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    int res = access("test.c", F_OK);
    printf("%i\n", res);
}