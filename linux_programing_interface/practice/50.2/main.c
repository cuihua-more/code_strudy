#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/mman.h>

static void errPrintf(int line, const char *fmt,...)
{
    va_list ap;
    char buf[4096];

    memset((void *)buf, 0, sizeof(buf));
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "line is %d ", line);
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "%s \n", strerror(errno));

    write(STDERR_FILENO, (void *)buf, strlen(buf));
    exit(EXIT_FAILURE);
}

#define ERROUT(...) errPrintf(__LINE__, __VA_ARGS__)

int main(char argc, char *argv[])
{
    int fd;
    char *p = NULL;

    fd = open("temp.txt", O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);
    if (fd == -1) {
        ERROUT("create temp.txt failed!");
    }

    write(fd, "abc", strlen("abc"));
    close(fd);

    fd = open("temp.txt", O_RDWR);
    if (fd == -1) {
        ERROUT("open temp.txt failed!");
    }

    p = mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE,
             MAP_PRIVATE, fd, 0);
    if (p == NULL) {
        ERROUT("mmap failed! ");
    }

    *(p + 1) = 'g';

    printf("%c\n", *(p + 1));

    /*
     * 当mmap的类型是私有时
     * madvise向内核建议MADV_DONTNEED时，
     * 会显式的丢弃当前修改的内容
     */
    if (madvise(p, 10, MADV_DONTNEED) == -1) {
        ERROUT("madvise failed!");
    }

    printf("%c\n", *(p + 1));

    munmap(p, sysconf(_SC_PAGE_SIZE));
}