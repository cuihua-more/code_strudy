#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <myDef.h>
#include "common.h"
#include <sys/types.h>
#include <unistd.h>

#define ERROUT(...) errPrintf(__FILE__, __LINE__, strerror(errno), __VA_ARGS__)

int main(char argc, char *argv[])
{
    int lfd;
    int type;
    ssize_t rdNum, wrNum;
    char buf[BUF_SIZE];

    if ((argc != 3) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s tcp/ucp data\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "tcp") == 0) {
        type = SOCK_STREAM;
    } else if (strcmp(argv[1], "udp") == 0) {
        type = SOCK_DGRAM;
    } else {
        fprintf(stderr, "Usage: %s tcp/ucp data\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lfd = inetConnect("Localhost", SERVER, type);
    if (lfd == -1) {
        ERROUT("inetConnect failed!");
    }

    wrNum = strlen(argv[2]);
    if (write(lfd, argv[2], wrNum) != wrNum) {
        ERROUT("write failed!");
    }

    rdNum = read(lfd, (void *)buf, sizeof(buf));
    if (rdNum < 0) {
        ERROUT("read failed!");
    }

    printf("Receive: %s \n", buf);

    exit(EXIT_SUCCESS);
}