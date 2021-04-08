#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


int main(char argc, char *argv[])
{
    int fd;
    char template[] = "./somestringXXXXXX";

    fd = mkstemp(template);
    if (fd == -1) {
        fprintf(stderr, "mkstemp failed!\n");
        exit(EXIT_FAILURE);
    }

    printf("Generated filename was:%s \n", template);
    unlink(template);

    close(fd);

    exit(EXIT_SUCCESS);
}