#define _GNU_SOURCE     /* To get various declarations from stdlib.h */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char **environ;

int main(char argc, char *argv[])
{
    int i;
    char **ep;

    clearenv();

    for (i = 1; i < argc; i++) {
        if (putenv(argv[i]) != 0) {
            fprintf(stderr, "putenv error!\n");
            exit(EXIT_FAILURE);
        }
    }

    if (setenv("GREET", "Hello World", 0) == -1) {
        fprintf(stderr, "setenv error!\n");
        exit(EXIT_FAILURE);
    }

    unsetenv("BYE");

    for (ep = environ; *ep != NULL; ep++) {
        puts(*ep);
    }

    exit(EXIT_SUCCESS);
}