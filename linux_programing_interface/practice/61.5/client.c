#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <myDef.h>
#include "common.h"
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ERROUT(...) errPrintf(__FILE__, __LINE__, strerror(errno), __VA_ARGS__)

int main(char argc, char *argv[])
{
    int lfd;
    char buf[BUF_SZIE];
    ssize_t numRd;
    int len;

    if ((argc < 3) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s host cmd\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lfd = inetConnect(argv[1], SERVER, SOCK_STREAM);
    if (lfd == -1) {
        ERROUT("inetConnect failed!");
    }

    memset((void *)buf, 0, sizeof(buf));
    for (int i = 2; i < argc; i++) {
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " %s", argv[i]);
    }

    len = strlen(buf);

    if (write(lfd, buf, len) != len) {
        ERROUT("write failed!");
    }

    memset((void *)buf, 0, sizeof(buf));
    numRd = read(lfd, (void *)buf, sizeof(buf));
    if (numRd < 0) {
        ERROUT("read failed!");
    }

    buf[numRd] = '\0';
    printf("%s", buf);

    exit(EXIT_SUCCESS);
}