#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int mydup(int oldfd)
{
    int newfd;

    newfd = fcntl(oldfd, F_DUPFD, 0);

    return newfd;
}

int mydup2(int oldfd, int newfd)
{
    int tempFd;
    int flags;

    if (oldfd == newfd) {
        if ((flags = fcntl(oldfd, F_GETFL)) != 0) {
            return newfd;
        }
        else {
            errno = EBADF;
            return -1;    
        }
    }

    close(newfd);
    tempFd = fcntl(oldfd, F_DUPFD, newfd);

    return tempFd;
}
