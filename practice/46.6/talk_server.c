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

struct clientInfo {
    int clientId;
    char clientName[CLIENT_NAME_LEN];
};

struct clientInfo pCi[CLIENT_NUM_MAX];

static int serverId;

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
    msgctl(serverId, IPC_RMID, NULL);
    remove(SERVER_ID_FILE_PATH);
}

static void SIGCHLD_Handler(int signal)
{
    int errSave;

    errSave = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
    errno = errSave;
}

static void sig_Handler(int signal) 
{
    exit(EXIT_FAILURE);
}

int main(char argc, char *argv[])
{
    int serverFd;
    char recvBuf[sizeof(struct taskMsg)];
    int msgLen;
    int clientCount = 0;
    struct sigaction sa;
    int i;

    serverId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL |
                                       S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);
    if (serverId == -1) {
        errPrintf(__LINE__, "msgget failed!");
    }

    atexit(onExit);

    serverFd = open(SERVER_ID_FILE_PATH, O_CREAT | O_WRONLY,
                    S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);
    if (serverFd == -1) {
        errPrintf(__LINE__, "open server fd file failed! %s", SERVER_ID_FILE_PATH);
    }

    if (write(serverFd, (void *)&serverId, sizeof(serverId)) != sizeof(serverId)) {
        close(serverFd);
        errPrintf(__LINE__, "write server fd file failed! %s", SERVER_ID_FILE_PATH);
    }
    close(serverFd);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = SIGCHLD_Handler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        errPrintf(__LINE__, "sigaction failed!");
    }

    signal(SIGINT, sig_Handler);
    signal(SIGTERM, sig_Handler);

    memset((void *)pCi, 0, sizeof(pCi));

    while (1) {
        memset(recvBuf, 0, sizeof(recvBuf));
        msgLen = msgrcv(serverId, (void *)&recvBuf, SERVER_MSG_LEN, 0, 0);
        if (msgLen == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                errPrintf(__LINE__, "msgrcv failed!");
            }
        }

        switch (((struct taskMsg *)recvBuf)->mtype) {
            case MTYPE_REGISTER: {
                /* register */
                clientCount++;
                struct registerMsg *reg = (struct registerMsg *)recvBuf;

                for (i = 0; i < CLIENT_NUM_MAX; i++) {
                    if (pCi[i].clientId == 0) {
                        break;
                    }
                }

                if (i == CLIENT_NUM_MAX) {
                    errPrintf(__LINE__, "client register num MAX!");
                }

                pCi[i].clientId = reg->clientId;
                strncpy(pCi[i].clientName, reg->clientName, CLIENT_NAME_LEN);

                printf("current has %d rember,i = %d, register Info: clientId = 0x%X, clientName = %s \n",
                    clientCount,
                    i,
                    pCi[i].clientId,
                    pCi[i].clientName);
                  
                break;
            } 
            case MTYPE_DELETE: {
                /* delete */
                clientCount--;
                struct registerMsg *reg = (struct registerMsg *)recvBuf;

                for (i = 0; i < CLIENT_NUM_MAX; i++) {
                    if (pCi[i].clientId == reg->clientId) {
                        pCi[i].clientId = 0;
                        memset((void *)pCi[i].clientName, 0, sizeof(CLIENT_NAME_LEN));
                        printf("delete clienId = 0x%x 's rember! current has %d \n",
                            reg->clientId, clientCount);
                        break;
                    }
                }

                if (i == CLIENT_NUM_MAX) {
                    printf("delete clienId = 0x%x 's rember failed! \n", reg->clientId);
                }
                break;
            }
            case MTYPE_TALK: {
                struct taskMsg *recv = (struct taskMsg *)recvBuf;

                for (i = 0; i < CLIENT_NUM_MAX; i++) {
                    if (strcmp(pCi[i].clientName, recv->targetName) == 0) {
                        break;
                    }
                }

                if (i == CLIENT_NUM_MAX) {
                    fprintf(stderr, "Server: Target Name not find!\n");
                    continue;
                }

                fprintf(stderr, "Server %s's Message: %s\n", recv->targetName, recv->msg);
                
                recv->mtype = MTYPE_TALK;

                if (msgsnd(pCi[i].clientId, (void *)recv, SERVER_MSG_LEN, 0) == -1) {
                    fprintf(stderr, "server msgsnd failed!\n");
                }
                break;
            }
            default:
                break;
        }
    }
}