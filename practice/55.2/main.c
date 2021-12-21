#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/file.h>

#define TEMP1_PATH  "temp1.txt"
#define TEMP2_PATH  "temp2.txt"

/*
 * 两个进程使用flock锁住两个不同的文件时，会造成死锁现象
 */
int main(char argc, char *argv[])
{
    int fd1, fd2;

    fd1 = open(TEMP1_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd1 == -1) {
        fprintf(stderr, "create %s failed! %s \n", TEMP1_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fd2 = open(TEMP2_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd2 == -1) {
        fprintf(stderr, "create %s failed! %s \n", TEMP2_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }
    close(fd1);
    close(fd2);

    switch(fork()) {
        case -1:
            fprintf(stderr, "fork failed!\n");
            exit(EXIT_FAILURE);
        case 0:
            printf("Child PID is %ld\n", (long)getpid());
            int fdC1, fdC2;
            fdC1 = open(TEMP1_PATH, O_RDWR);
            if (fdC1 == -1) {
                fprintf(stderr, "Child open %s failed! %s\n", TEMP1_PATH,
                        strerror(errno));
                exit(EXIT_FAILURE);
            }
            fdC2 = open(TEMP2_PATH, O_RDWR);
            if (fdC2 == -1) {
                fprintf(stderr, "Child open %s failed! %s\n", TEMP2_PATH,
                        strerror(errno));
                exit(EXIT_FAILURE);
            }

            if (flock(fdC1, LOCK_EX) == -1) {
                fprintf(stderr, "PID is %ld flock failed! line = %d %s \n",
                        (long)getpid(),
                        __LINE__,
                        strerror(errno));
                 exit(EXIT_FAILURE);   
            }

            printf("PID is %ld get %s file lock\n", (long)getpid(), TEMP1_PATH);

            sleep(1);

            if (flock(fdC2, LOCK_EX) == -1) {
                fprintf(stderr, "PID is %ld flock failed! line = %d %s \n",
                        (long)getpid(),
                        __LINE__,
                        strerror(errno));
                exit(EXIT_FAILURE);
            }
            printf("PID is %ld get %s file lock\n", (long)getpid(), TEMP2_PATH);

            exit(EXIT_SUCCESS);
        default:
            printf("Parent PID is %ld\n", (long)getpid());
            int fdP1, fdP2;
            fdP1 = open(TEMP1_PATH, O_RDWR);
            if (fdP1 == -1) {
                fprintf(stderr, "Parnet open %s failed! %s\n", TEMP1_PATH,
                        strerror(errno));
                exit(EXIT_FAILURE);
            }
            fdP2 = open(TEMP2_PATH, O_RDWR);
            if (fdP2 == -1) {
                fprintf(stderr, "Parnet open %s failed! %s\n", TEMP2_PATH,
                        strerror(errno));
                exit(EXIT_FAILURE);
            }
            if (flock(fdP2, LOCK_EX) == -1) {
               fprintf(stderr, "PID is %ld flock failed! line = %d %s \n",
                        (long)getpid(),
                        __LINE__,
                        strerror(errno));
                exit(EXIT_FAILURE);
            }
            printf("PID is %ld get %s file lock\n", (long)getpid(), TEMP2_PATH);

            sleep(1);

            if (flock(fdP1, LOCK_EX) == -1) {
                fprintf(stderr, "PID is %ld flock failed! line = %d %s \n",
                        (long)getpid(),
                        __LINE__,
                        strerror(errno));
                exit(EXIT_FAILURE);
            }
            printf("PID is %ld get %s file lock\n", (long)getpid(), TEMP1_PATH);
            break;
    }

    waitpid(-1, NULL, 0);
    exit(EXIT_SUCCESS);
}