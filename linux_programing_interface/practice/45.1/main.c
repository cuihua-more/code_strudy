#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <stdarg.h>
#include <string.h>
#include <linux/kdev_t.h>

#define TMEP_PATH   "temp.txt"

static void errPrintf(int line, const char *fmt, ...)
{
    va_list ap;
    char buf[4096];

    memset(buf, 0, sizeof(buf));
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "line = %d\n",
             line);
    write(STDERR_FILENO, (void *)buf, strlen(buf));

    exit(EXIT_FAILURE);
}

int main(char argc, char *argv[])
{
    key_t key;
    int fd;
    struct stat status;
    short inode;
    char devNum;
    char proj;
    int calcu;

    if ((argc > 1) && (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s proj\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    proj = (argc > 1) ? (char)atoi(argv[1]) : 1;

    fd = open(TMEP_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd == -1) {
        errPrintf(__LINE__, "open temp.txt file failed!");
    }

    close(fd);

    key = ftok(TMEP_PATH, proj);
    if (key == -1) {
        errPrintf(__LINE__, "ftok failed!");
    }
    printf("ket is 0x%X\n", (int)key);

    if (stat(TMEP_PATH, &status) == -1) {
        errPrintf(__LINE__, "stat failed!");
    }

    inode = (short)status.st_ino;
    devNum = MINOR(status.st_dev);
    
    printf("proi is 0x%X, minDev is 0x%x, file inode is 0x%X\n", proj, devNum, inode);

    calcu = (proj << 24) | (devNum << 16) | (inode << 0);
    printf("calculate key num is 0x%X\n", calcu);

    printf("key is%s equal of calculate key\n", (calcu == (int)key) ? "" : "n't");

    exit(EXIT_SUCCESS);
}