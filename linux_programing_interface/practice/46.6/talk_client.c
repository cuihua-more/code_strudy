#define _GNU_SOURCE
#include "talk_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

static int clientId, serverId;

static void errPrintf(int line, const char *fmt,...)
{
    va_list ap;
    char buf[4096];

    memset((void *)buf, 0, sizeof(buf));
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " line = %d \n", line);

    exit(EXIT_FAILURE);
}

static void onExit(void)
{
    struct registerMsg reg;

    reg.mtype = MTYPE_DELETE;
    reg.clientId = clientId;
    msgsnd(serverId, (void *)&reg, SERVER_REG_LEN, 0);

    msgctl(clientId, IPC_RMID, NULL);
}

static void sig_Handler(int signal)
{
    exit(EXIT_FAILURE);
}

static void *recvServerMsg(void *arg)
{
     struct taskMsg msg;
    int msgLen;

    while (1) {
        msgLen = msgrcv(serverId, (void *)&msg, SERVER_MSG_LEN, MTYPE_TALK, 0);
        if (msgLen == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                errPrintf(__LINE__, "msgrcv failed!");
            }
        }

        system("clear");
        printf("\nRecv %s's Message: %s\n", msg.targetName, msg.msg);
        memset((void *)&msg, 0, sizeof(msg));
    }
}

int main(char argc, char *argv[])
{
    struct registerMsg reg;
    int serverFd;

    if ((argc != 2) || (strcmp(argv[1], "--help") == 0)) {
        errPrintf(__LINE__, "Usage: %s YourName");
    }

    if (strlen(argv[1]) > CLIENT_NAME_LEN) {
        errPrintf(__LINE__, "Your Name is too len!");
    }

    serverFd = open(SERVER_ID_FILE_PATH, O_RDONLY);
    if (serverFd == -1) {
        errPrintf(__LINE__, "open server message queue Id File failed!");
    }

    if (read(serverFd, (void *)&serverId, sizeof(serverId)) != sizeof(serverId)) {
        close(serverFd);
        errPrintf(__LINE__, "read server message queue Id File failed!");
    }

    close(serverFd);

    clientId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL |
                                       S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);
    if (clientId == -1) {
        errPrintf(__LINE__, "msgget client Message Queue Id failed!");
    }

    atexit(onExit);

    signal(SIGINT, sig_Handler);
    signal(SIGTERM, sig_Handler);

    reg.mtype = MTYPE_REGISTER;
    reg.clientId = clientId;
    strncpy(reg.clientName, argv[1], CLIENT_NAME_LEN);

    if (msgsnd(serverId, (void *)&reg, SERVER_REG_LEN, 0) == -1) {
        errPrintf(__LINE__, "msgsnd register info failed!\n");
    }

    switch (fork()) {
        case -1:
            errPrintf(__LINE__, "fork failed!");
            break;    
        case 0: {
            struct taskMsg msg;
            int msgLen;
            while (1) {
                msgLen = msgrcv(serverId, (void *)&msg, SERVER_MSG_LEN, MTYPE_TALK, 0);
                if (msgLen == -1) {
                    if (errno == EINTR) {
                        continue;
                    } else {
                        errPrintf(__LINE__, "msgrcv failed!");
                    }
                }

                printf("\nRecv %s's Message: %s\n", msg.targetName, msg.msg);
                memset((void *)&msg, 0, sizeof(msg));
            }
            break;
        }
        default:
            break;
    }

    char buf[MESSAGE_LEN];
    struct taskMsg msg;

    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    while (1) {
        memset((void *)&msg, 0, sizeof(msg));
        fflush(stdin);
        fprintf(stdout, "Plese enter Message (Name Message\\n): \n");
        scanf("%s %s\n", msg.targetName, msg.msg);

        fprintf(stderr, "\n Current send %s's Message: %s\n",
                msg.targetName,
                msg.msg);
        msg.mtype = MTYPE_TALK;
        if (msgsnd(serverId, (void *)&msg, SERVER_MSG_LEN, 0) == -1) {
            errPrintf(__LINE__, "msgsend message faield!");
        }
    }

    exit(EXIT_SUCCESS);
}