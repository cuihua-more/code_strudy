#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>


int main(char argc, char *argv[])
{
    int oldfd, newfd;
    int numRd, numWr;
    char buf[100];

    if (argc < 3) {
        fprintf(stderr, "%s oldfile newfile \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    oldfd = open(argv[1], O_RDONLY);
    if (oldfd == -1) {
        fprintf(stderr, "%s not exist!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    newfd = open(argv[2], O_RDWR | O_CREAT, S_IRWXU);
    if (newfd == -1) {
        close(oldfd);
        fprintf(stderr, "%s create failed!\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    while ((numRd = read(oldfd, buf, 100)) != 0) {
        numWr = write(newfd, buf, numRd);
        if (numWr != numRd) {
            close(oldfd);
            remove(argv[2]);
            fprintf(stderr, "write %s failed!\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    close(oldfd);
    close(newfd);

    exit(EXIT_SUCCESS);
}
