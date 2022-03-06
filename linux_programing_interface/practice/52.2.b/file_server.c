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
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

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

static void notifySetup(mqd_t *mqdp);

static int msgHandler(struct mq_request *req)
{
    struct mq_response resp;
    mqd_t clientFd;
    int fd;
    int num = 0;

    printf("%d \n", __LINE__);
    
    printf("server: client_mq_name = %s client_pathname = %s \n", req->client_mq_name, req->file_name);
    
    clientFd = mq_open(req->client_mq_name, O_RDWR);
    if (clientFd == -1) {
        ERROUT("mq_open client failed!");
    }

    printf("%d \n", __LINE__);


    fd = open(req->file_name, O_RDONLY);
    if (fd == -1) {
        resp.flags = RESP_FAILE;
        mq_send(clientFd, (char *)&resp, sizeof(resp), 0);
        mq_close(clientFd);
        ERROUT("open file failed! ");
    }

    while (1) {
        resp.dataLen = read(fd, resp.data, sizeof(resp.data));
        if (resp.dataLen == 0) {
            printf("server: send end!");
            break;
        }
        resp.flags = RESP_DATA;
        if (mq_send(clientFd, (char *)&resp, sizeof(resp), 0) == -1) {
            mq_close(clientFd);
            ERROUT("mq_send client failed!");
        }
        num += resp.dataLen;
    }

    printf("server Send %d Bytes \n", num);

    close(fd);

    resp.flags = RESP_END;
    mq_send(clientFd, (char *)&resp, sizeof(resp), 0);
    mq_close(clientFd);
}

static void threadFunc(union sigval sv)
{
    struct mq_request req;
    struct mq_attr attr;
    ssize_t recvSize;
    unsigned int prio;
    mqd_t *serverFd;

    serverFd = (mqd_t *)sv.sival_ptr;

    printf("enetr thread %ld \n", (long)gettid());

    if (mq_getattr(*serverFd, &attr) == -1) {
        ERROUT("mq_getattr server failed!");
    }

    notifySetup(serverFd);

    recvSize = mq_receive(*serverFd, (char *)&req, attr.mq_msgsize, &prio);
    if (recvSize == -1) {
        ERROUT("mq_receive server failed!");
    }
    printf("server: client_mq_name = %s client_pathname = %s \n", req.client_mq_name, req.file_name);
    msgHandler(&req);

    pthread_exit(NULL);
}

static void notifySetup(mqd_t *mqdp)
{
    struct sigevent event;

    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = threadFunc;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = (void *)mqdp;

    if (mq_notify(*mqdp, &event) == -1) {
        ERROUT("mq_notify failed!");
    }
}

int main(char argc, char *argv[])
{
    mqd_t serverFd;

    serverFd = mq_open(SERVER_MQ, O_CREAT | O_EXCL | O_RDWR, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP, NULL);
    if (serverFd == -1) {
        ERROUT("mq_open server failed!");
    }

    notifySetup(&serverFd);

    pause();

    mq_close(serverFd);
    mq_unlink(SERVER_MQ);

    exit(EXIT_SUCCESS);
}