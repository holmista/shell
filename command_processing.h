#ifndef COMMAND_PROCESSING_H
#define COMMAND_PROCESSING_H

char* trimLeading(char* string);
char* trimTrailing(char* string);
char* trimLeadingAndTrailing(char* string);
char** parseInput(char* input);
char** parseCommand(char* command);
void freeDoubleCharPointer(char** ptr);
int doubleCharPointerLength(char** ptr);
int commandContainsRedirection(char** ptr);
int length(char**);
char* getCommandFullPath(char** directories, char* command);

#endif