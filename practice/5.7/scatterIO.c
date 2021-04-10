#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "scatterIO.h"

ssize_t myreadv(int fd, const struct myiovec *iov, int iovcnt)
{
    int i;

    for (i = 0; i < iovcnt; i++) {
        if (!iov[i].iov_base) {
            fprintf(stderr,"iov[%d].iov_base is null\n", i);
            return -1;
        }
        if (read(fd, iov[i].iov_base, iov[i].iov_len) < 0) {
            fprintf(stderr, "read iov[%d] failed!\n", i);
            exit(EXIT_FAILURE);
        }
    }

    return i;
}

ssize_t mywritev(int fd, const struct myiovec *iov, int iovcnt)
{
    int i;

    for (i = 0; i < iovcnt; i++) {
        if (!iov[i].iov_base) {
            fprintf(stderr,"iov[%d].iov_base is null\n", i);
            return -1;
        }
        if (write(fd, iov[i].iov_base, iov[i].iov_len) < 0) {
            fprintf(stderr, "write iov[%d] failed!\n", i);
            exit(EXIT_FAILURE);
        }
    }

    return i;
}