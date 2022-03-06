#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int mydup(int oldfd);
int mydup2(int oldfd, int newfd);

int main(char argc, char *argv[])
{
    int fd, fd1, fd2;
    char buf[] = "test";
    char temp[5];

    if (argc < 2) {
        fprintf(stderr, "%s file \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        fprintf(stderr, "%s create failed!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    printf("fd = %d\n", fd);

    write(fd, buf, sizeof(buf));

    fd1 = mydup(fd);
    if (fd1 != -1) {
        printf("mydup fd1 = %d\n", fd1);
        lseek(fd1, 0, SEEK_SET);
        if (read(fd1, temp, sizeof(temp) - 1)) {
            printf("temp: %s\n", temp);
        }
    }

    fd2 = 2021;
    fd2 = mydup2(fd, fd2);
    if (fd2!= -1) {
        printf("mydup2 fd2 = %d\n", fd2);
        lseek(fd2, 0, SEEK_SET);
        if (read(fd2, temp, sizeof(temp) - 1)) {
            printf("temp: %s\n", temp);
        }
    }

    close(fd);

    lseek(fd1, 0, SEEK_SET);
    if (read(fd1, temp, sizeof(temp) - 1)) {
        printf("temp: %s\n", temp);
        close(fd1);
    }

    lseek(fd2, 0, SEEK_SET);
    if (read(fd2, temp, sizeof(temp) - 1)) {
        printf("temp: %s\n", temp);
        close(fd2);
    }

    exit(EXIT_SUCCESS);
}
