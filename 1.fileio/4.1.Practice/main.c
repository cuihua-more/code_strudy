#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(char argc, char *argv[])
{
    int fd;
    char buf[100];
    ssize_t numRd, numWr;

    if (argc < 2) {
        printf("Usage： %s file [-a]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((argc == 3) && (!strcmp(argv[2], "-a"))) {
        fd = open(argv[1], O_RDWR | O_APPEND);
    }
    else {
        fd = open(argv[1], O_RDWR | O_TRUNC);
    }

    if (fd == -1) {
        fd = open(argv[1], O_CREAT | O_RDWR, S_IRWXU);
        printf("create %s !\n",argv[1]);
        if (fd == -1) {
            printf("create %s failed!\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    //numRd = read(stdin, buf, sizeof(buf));
    
    while (fgets(buf, sizeof(buf), stdin)) {
        numWr = write(fd, buf, strlen(buf));
        if (numWr != strlen(buf)) {
            close(fd);
            printf("write %s failed!\n", argv[1]);
            exit(EXIT_FAILURE);
        } 
        if (fputs(buf, stdout)){
            close(fd);
            printf("write %s failed!\n", "stdout");
            exit(EXIT_FAILURE);
        }
    }

    close(fd);

    exit(EXIT_SUCCESS);
}