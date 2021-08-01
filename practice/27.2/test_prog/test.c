#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int main(char argc, char *argv[])
{
    int i;

    printf("Run Test Program ------------------------------------------\n");

    for (i = 0; i < argc; i++) {
        printf("Argv[%d] = %s\n", i, argv[i]);
    }

    for (i = 0; environ[i] != NULL; i++) {
        printf("Environ[%d] = %s\n", i, environ[i]);
    }

    printf("End Test Program ------------------------------------------\n");

    exit(EXIT_SUCCESS);
}