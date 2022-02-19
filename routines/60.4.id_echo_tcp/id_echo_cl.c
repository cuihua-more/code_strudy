#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <myDef.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "id_echo.h"

#define ERROUT(...) errPrintf(NULL, __LINE__, strerror(errno), __VA_ARGS__)

int main(char argc, char *argv[])
{
    int lfd;
    int size;
    ssize_t numRead;
    char buf[BUF_SIZE];

    if ((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s host msg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lfd = inetConnect(argv[1], SERVICE, SOCK_STREAM);
    if (lfd == -1) {
        ERROUT("inetConnect failed!\n");
    }


    for (int i = 2; i < argc; i++) {
        size = strlen(argv[i]);
        if (write(lfd, (void *)argv[i], size) != size) {
            close(lfd);
            ERROUT("write failed!");
        }

        numRead = read(lfd, (void *)buf, sizeof(buf));
        if (numRead == -1) {
            ERROUT("read failed!");
        }

        printf("[%ld bytes] %.*s\n", (long)numRead, (int)numRead, buf);
    }

    exit(EXIT_SUCCESS);
}