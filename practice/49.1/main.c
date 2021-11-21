#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

static void errPrintf(int line, const char *fmt, ...)
{
    va_list ap;
    char buf[4096];

    memset((void *)buf, 0, sizeof(buf));
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " line is %d ", line);
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " %s \n", strerror(errno));
    write(STDERR_FILENO, (void *)buf, strlen(buf));
    exit(EXIT_FAILURE);
}
#define ERROUT(...) errPrintf(__LINE__, __VA_ARGS__)

int main(char argc, char *argv[])
{
    int fromFd, toFd;
    char *pFrom = NULL, *pTo = NULL;
    struct stat stat;
    int len;

    if ((argc != 3) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s PathnameFrom PathnameTo \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fromFd = open(argv[1], O_RDWR);
    if (fromFd == -1) {
        ERROUT("open Path %s failed!", argv[1]);
    }

    if (fstat(fromFd, &stat) == -1) {
        ERROUT("fstat failed!");
    }

    len = (int)stat.st_size;

    pFrom = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                 fromFd, 0);
    if (pFrom == MAP_FAILED) {
        ERROUT("mmap from file failed!");
    }

    close(fromFd);

    toFd = open(argv[2], O_CREAT | O_RDWR, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);
    if (toFd == -1) {
        ERROUT("create Path %s failed!", argv[2]);
    }

    if (ftruncate(toFd, stat.st_size) == -1) {
        ERROUT("ftruncate failed!");
    }

    pTo = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                 toFd, 0);
    if (pTo == MAP_FAILED) {
        ERROUT("mmap from file failed!");
    }

    close(toFd);

    memcpy(pTo, pFrom, stat.st_size);
    msync(pTo, stat.st_size, MS_SYNC);

    munmap(pFrom, stat.st_size);
    munmap(pTo, stat.st_size);

    exit(EXIT_SUCCESS);
}