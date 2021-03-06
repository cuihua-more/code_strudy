#define _GNU_SOURCE
#include "svmsg_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

static void SIGCHILD_Hanler(int signal)
{
    int errnoSave;

    errnoSave = errno;
    while (waitpid(-1, NULL, WNOHANG) == -1) {
        continue;
    }
    errno = errnoSave;
}

static void serverRequest(struct requestMsg *msg)
{
    struct responseMsg resp;
    int fd;
    int readLen;

    fd = open(msg->pathName, O_RDONLY);
    if (fd == -1) {
        resp.mtype = RESP_MT_FAILURE;
        sprintf(resp.data, "coun't open fail!\n");
        msgsnd(msg->chiletId, (void *)&resp, strlen(resp.data), 0);
        exit(EXIT_FAILURE);
    }

    resp.mtype = RESP_MT_DATA;
    while ((readLen = read(fd, (void *)resp.data, RESP_MSG_SIZE)) > 0) {
        if (msgsnd(msg->chiletId, (void *)&resp, readLen, 0) == -1) {
            break;
        }
    }

    /* send a message of type RESP_MT_END to signify end-of-file */
    resp.mtype = RESP_MT_END;
    msgsnd(msg->chiletId, (void *)&resp, strlen(resp.data), 0);
}

int main(char argc, char *argv[])
{
    struct requestMsg req;
    int serverId;
    struct sigaction sa;
    int msgLen;
    pid_t pid;

    /* create server message queue */
    while ((serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL |
                                      S_IRUSR | S_IWUSR | S_IWGRP)) == -1) {
        if (errno == EEXIST) {
            serverId = msgget(SERVER_KEY, 0);
            if (serverId != -1) {
                if (msgctl(serverId, IPC_RMID, NULL) != -1) {
                    printf("Removed old message queue (id = %d)\n", serverId);
                    continue;
                }
            }
        }
        fprintf(stderr, "msgget failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags     = SA_RESTART;
    sa.sa_handler = SIGCHILD_Hanler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    while (1) {
        msgLen = msgrcv(serverId, (void *)&req, REQ_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
            if (errno == EINTR) {
                continue;
            }
            fprintf(stderr, "msgrcv failed! line = %d\n", __LINE__);
            break;
        }

        pid = fork();
        if (pid == -1) {
            fprintf(stderr, "pid faild! line = %d\n", __LINE__);
            break;
        }

        if (pid == 0) {
            serverRequest(&req);
            _exit(EXIT_SUCCESS);
        }
    }

    /* delete server message queue */
    msgctl(serverId, IPC_RMID, NULL);

    exit(EXIT_SUCCESS);
}