#define _FILE_OFFSET_BITS   64
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(char argc, char *argv[])
{
    int fd;
    loff_t off;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s file offset \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_CREAT | O_RDWR, S_IRWXU);
    if (fd == -1) {
        fprintf(stderr, "open failed! \n");
        exit(EXIT_FAILURE);
    }

    off = atoll(argv[2]);
    if (lseek(fd, off, SEEK_CUR) == -1) {
        fprintf(stderr, "lseek failed! \n");
        exit(EXIT_FAILURE);
    }

    if (write(fd, "test", 4) == -1) {
        fprintf(stderr, "write failed! \n");
        exit(EXIT_FAILURE);
    }

    close(fd);
    
    exit(EXIT_SUCCESS);
}