#define _FILE_OFFSET_BITS   64
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(char argc, char *argv[])
{
    int fd;
    int flags;
    ssize_t count;
    char uselseek;

    if (argc < 2) {
        fprintf(stderr, "%s filename num-bytes [x] \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    count = atoll(argv[2]);

    uselseek = (argc > 3);

    flags = uselseek ? 0 : O_APPEND;

    fd = open(argv[1], O_RDWR | O_CREAT | flags, S_IRUSR | S_IWUSR);
    if (fd == -1 ) {
        printf("%s not exist!\n", argv[1]);  
        exit(EXIT_FAILURE);
    }


    for (count; count > 0; count--) {
        if (uselseek) {
            if (lseek(fd, 0, SEEK_END) == -1) {
                fprintf(stderr, "lseek failed!\n");
                exit(EXIT_FAILURE);
            }
        }
        if (write(fd, "x", 1) != 1) {
            fprintf(stderr, "write failed!\n");
            exit(EXIT_FAILURE);
        }
    }

    close(fd);

    printf("%ld done\n", (long)getpid());
    exit(EXIT_SUCCESS);
}