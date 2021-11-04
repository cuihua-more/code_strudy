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
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct response resp;
    struct request req;
    int reqNum = 0;
    int count = 0;

    umask(0);

    if ((mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1) && (errno != EEXIST)) {
        fprintf(stderr, "mkfifo failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    serverFd = open(SERVER_FIFO, O_RDONLY);     /* 此处，没有人读取，所以打开FIFO会阻塞到这 */
    if (serverFd == -1) {
        fprintf(stderr, "open fifo failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    dummyFd = open(SERVER_FIFO, O_WRONLY);      /* 再次打开目的是为了循环read fifo时，不会读到文件结尾符 */
    if (dummyFd == -1) {
        fprintf(stderr, "open fifo failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "signal failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    while (1) {
        count++;
        if (count == 3) {
            break;
        }

        if (read(serverFd, (void *)&req, sizeof(req) != sizeof(req))) {
            fprintf(stderr, "Erroe read request!\n");
            continue;
        }
        
        memset((void *)clientFifo, 0, sizeof(clientFifo));

        snprintf(clientFifo, sizeof(clientFifo), CLIENT_FIFO_TEMPLATE, (unsigned long)req.pid);
        printf("client req pid = %ld. reqNum = %d\n %s\n", (long)req.pid, req.seqLen, clientFifo);
        clientFd = open(clientFifo, O_WRONLY);
        if (clientFd == -1) {
            fprintf(stderr, "open client fifo failed! path = %s, line = %d\n", clientFifo, __LINE__);
            continue;
        }


        reqNum += req.seqLen;
        resp.seqLen = reqNum;

        if (write(clientFd, (void *)&resp, sizeof(resp)) != sizeof(resp)) {
            fprintf(stderr, "write client fifo failed! path = %s, line = %d\n", clientFifo, __LINE__);
            close(clientFd);
            continue;
        }

        close(clientFd);
    }

    exit(EXIT_FAILURE);
}