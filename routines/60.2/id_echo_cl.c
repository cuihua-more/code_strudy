#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <myDef.h>
#include <syslog.h>
#include "id_echo.h"
#include "inet_sockets.h"

#define ERROUT(...) errPrintf(NULL, __LINE__, strerror(errno), __VA_ARGS__)

int main(char argc, char *argv[])
{
    int len, numRead;
    int lfd;
    char buf[BUF_SIZE];

    if ((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s host msg...\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    lfd = inetConnect(argv[1], SERVER, SOCK_DGRAM);
    if (lfd == -1) {
        ERROUT("inetConnect failed!");
    }

    for (int i = 2; i < argc; i++) {
        len = strlen(argv[i]);
        if (write(lfd, argv[i], len) != len) {
            ERROUT("write failed!");
        }

        numRead = read(lfd, (void *)buf, sizeof(buf));
        if (numRead == -1) {
            ERROUT("read faild!");
        }

        printf("[%ld bytes] %.*s\n", (long)numRead, (int)numRead, buf);
    }

    exit(EXIT_SUCCESS);
}