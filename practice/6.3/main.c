#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int myunsetenv(const char *name);
int mysetenv(const char *name, const char *value, int overwrite);

void printenv()
{
    extern char ** environ;
    char **ep;

    printf("current env:\n");

    for (ep = environ; *ep != NULL; ep++) {
        puts(*ep);
    }
}

int main(char argc, char *argv[])
{
    //clearenv();

    if (mysetenv("name", "xxxxxx", 0) == 0) {
        printenv();
    }
    else {
        fprintf(stderr, "mysetenv 0 failed!\n");
        exit(EXIT_FAILURE);
    }

    if (mysetenv("name", "xxxxx1", 1) == 0) {
        printenv();
    }
    else {
        fprintf(stderr, "mysetenv 1 failed!\n");
        exit(EXIT_FAILURE);
    }

    if (mysetenv("test5", "1234", 0) == 0) {
        printenv();
    }
    else {
        fprintf(stderr, "mysetenv 0 failed!\n");
        exit(EXIT_FAILURE);
    }
    putenv("test2=teste");
    //myunsetenv("name");

    putenv("test1=teste");
    putenv("test3=teste");
    

    printenv();

    exit(EXIT_SUCCESS);
}