#define _GNU_SOURCE
#include "fifo_seqnum_common.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

int main(char argc, char *argv[])
{
    int serverFd, clientFd;
    struct request req;
    struct response resp;
    char clientFifo[CLIENT_FIFO_NAME_LEN];

    umask(0);

    memset((void *)clientFifo, 0, sizeof(clientFifo));
    snprintf(clientFifo, sizeof(clientFifo), CLIENT_FIFO_TEMPLATE, (long)getpid());

    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1) {
        fprintf(stderr, "mkfifo failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1) {
        fprintf(stderr, "open server fifo failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    req.pid = (long)getpid();
    req.seqLen = (argc > 1) ? atol(argv[1]) : 1;

    if (write(serverFd, (void *)&req, sizeof(req)) != sizeof(req)) {
        fprintf(stderr, "write server fifo failed! path = %s line = %d\n", SERVER_FIFO, __LINE__);
        exit(EXIT_FAILURE);
    }

    clientFd = open(clientFifo, O_RDONLY);
    if (clientFd == -1) {
        fprintf(stderr, "open client fifo failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    if (read(clientFd, (void *)&resp, sizeof(resp)) != sizeof(resp)) {
        fprintf(stderr, "read client fifo failed! path = %s line = %d\n", clientFifo, __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("resp is %d\n", resp.seqLen);
    exit(EXIT_SUCCESS);
}