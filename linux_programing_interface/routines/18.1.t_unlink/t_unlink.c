#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#define BUF_SIZE        1024
#define CMD_SIZE        1024

static void usageErr(const char *format,...)
{
    va_list ap;
    char buf[1024];

    va_start(ap, format);
    vsnprintf(buf, 1024, format, ap);
    va_end(ap);

    fprintf(stderr, "Usage Error: %s ", buf);

    exit(EXIT_FAILURE);
}

static void errExit(const int line, const char *format,...)
{
    va_list ap;
    char buf[1024];

    va_start(ap, format);
    vsnprintf(buf, 1024, format, ap);
    va_end(ap);

    fprintf(stderr, "%s line: %d\n", buf, line);

    exit(EXIT_FAILURE);
}

int main(char argc, char *argv[])
{
    int numBlocks;
    int fd, i;
    char buf[BUF_SIZE] = "abcdefg";
    char cmd[CMD_SIZE];

    if ((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
        usageErr("%s temp-file [num-blocks] \n", argv[0]);
    }

    numBlocks = (argc > 2) ? atol(argv[2]) : 10000;

    fd = open(argv[1], O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        errExit(__LINE__, "open failed!");
    }

    if (unlink(argv[1]) == -1){
        errExit(__LINE__, "unlink failed!");
    }

    for (i = 0; i < numBlocks; i++) {
        if (write(fd, buf, BUF_SIZE) != BUF_SIZE) {
            close(fd);
            errExit(__LINE__, "write failed at %d times!", i);
        }
    }

    sprintf(cmd, "df -k `dirname %s`", argv[1]);
    system(cmd);

    if (close(fd) == -1) {
         errExit(__LINE__, "close failed!");
    }

    printf("*************Closed file descriptor\n");

    system(cmd);

    exit(EXIT_SUCCESS);
}