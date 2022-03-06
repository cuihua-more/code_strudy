#define _GNU_SOURCE
#include "file_comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <errno.h>
#include <stdarg.h>

void errPrintf(char *file, int line, char *fmt,...)
{
    va_list ap;
    char buf[4096];

    memset((char *)buf, 0, sizeof(buf));

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " file is %s ", file);
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " line is %d ", line);
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " %s\n ", strerror(errno));

    write(STDERR_FILENO, buf, strlen(buf));

    exit(EXIT_FAILURE);
}

int main(char argc, char *argv[])
{
    struct mq_request req;
    struct mq_response resp;
    int serverFd, clientFd;
    struct mq_attr attr;
    ssize_t recvLen;
    unsigned int prio;
    int fd;
    int num = 0;

    if ((argc != 4) || strcmp(argv[2], "--help") == 0) {
        fprintf(stderr, "Usage: %s mq_name file_name", argv[0]);
        exit(EXIT_FAILURE);
    }

    clientFd = mq_open(argv[1], O_RDWR | O_CREAT | O_EXCL,
                       S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, NULL);
    if (clientFd == -1) {
        ERROUT("mq_open client failed!");
    }

    strcpy(req.client_mq_name, argv[1]);
    strcpy(req.file_name, argv[2]);

    serverFd = mq_open(SERVER_MQ, O_RDWR);
    if (serverFd == -1) {
        ERROUT("mq_open servser failed!");
    }

    if (mq_getattr(serverFd, &attr) == -1) {
        ERROUT("mq_getattr failed!");
    }

    mq_send(serverFd, (char *)&req, attr.mq_msgsize, 0);

    if (mq_getattr(clientFd, &attr) == -1) {
        ERROUT("mq_getattr failed!");
    }

    fd = open(argv[3], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd == -1) {
        ERROUT("create %s failed!", argv[3]);
    }

    while (1) {
        recvLen = mq_receive(clientFd, (void *)&resp, attr.mq_msgsize, &prio);
        if (recvLen == 0) {
            fprintf(stderr, "client: mq_receive failed! \n");
            break;
        }
        if (resp.flags != RESP_DATA) {
            printf("client: receive end!\n");
            break;
        }
        write(fd, (void *)resp.data, resp.dataLen);
        num += resp.dataLen;
    }

    printf("client: receive %d Bytes\n", num);

    close(fd);

    mq_close(serverFd);
    mq_close(clientFd);
    mq_unlink(argv[1]);

    exit(EXIT_FAILURE);
}