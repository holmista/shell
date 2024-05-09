#include "string.h"
#include <unistd.h>

void printError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}