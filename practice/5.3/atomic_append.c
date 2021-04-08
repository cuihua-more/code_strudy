#define _FILE_OFFSET_BITS   64
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(char argc, char *argv[])
{
    int fd;

    if (argc < 2) {
        fprintf(stderr, "%s filename num-bytes [x] \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    

    exit(EXIT_SUCCESS);
}