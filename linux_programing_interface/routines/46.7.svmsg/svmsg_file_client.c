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

static int clientId;

void onExit(void)
{
    msgctl(clientId, IPC_RMID, NULL);
}

int main(char argc, char *argv[])
{
    struct requestMsg req;
    struct responseMsg resp;
    int serverId;
    int readNum, total, msgNum;

    if ((argc != 2) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s filepath \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strlen(argv[1]) > PATH_MAX) {
        fprintf(stderr, "Pathname: %s too long!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    serverId = msgget(SERVER_KEY, S_IWUSR);
    if (serverId == -1) {
        fprintf(stderr, "msgget server id failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    clientId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL |
                                       S_IWUSR | S_IRUSR | S_IWGRP);
    if (clientId == -1) {
        fprintf(stderr, "msgget client id failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    atexit(onExit);

    req.chiletId = clientId;
    req.mtype = 1;
    memset(req.pathName, 0, sizeof(req.pathName));
    strcpy(req.pathName, argv[1]);

    if (msgsnd(serverId, (void *)&req, REQ_MSG_SIZE, 0) == -1) {
        fprintf(stderr, "msgsnd failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    memset((void *)&resp, 0, sizeof(resp));

    msgNum = msgrcv(clientId, (void *)&resp, RESP_MSG_SIZE, 0, 0);
    if (msgNum == -1) {
        fprintf(stderr, "clientId failed! line = %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    if (resp.mtype == RESP_MT_FAILURE) {
        fprintf(stderr, "%s\n", resp.data);
        exit(EXIT_FAILURE);
    }

    total = msgNum;
    for (readNum = 1; resp.mtype == RESP_MT_DATA; readNum++) {
        msgNum = msgrcv(clientId, (void *)&resp, RESP_MSG_SIZE, 0, 0);
        if (msgNum == -1) {
            fprintf(stderr, "msgrcv failed! at %d times, line = %d\n", readNum, __LINE__);
            exit(EXIT_FAILURE);
        }
        if (resp.mtype == RESP_MT_DATA) {
            total += msgNum;
        }
    }

    printf("Receive %d bytes (%d messages)\n", total, readNum);

    exit(EXIT_SUCCESS);
}
