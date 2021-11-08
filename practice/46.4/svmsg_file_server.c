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
#include <syslog.h>

#define SERVER_LOG_INDET    "svmsg_server"

static int serverIdFd;

static void onExit(void)
{
    if (remove(SERVER_KEY_FILE) == -1) {
        fprintf(stderr, "remove file failed!\n line = %d\n", __LINE__);
    }
    closelog();
}

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

    /*
     * open log 
     */
    openlog(SERVER_LOG_INDET, LOG_CONS | LOG_PID |LOG_NOWAIT, LOG_USER);

    /*
     * 设置可以打印出的log等级，只有大于这个等级的消息才不会被丢弃 
     */
    //setlogmask(LOG_UPTO(LOG_WARNING));

    syslog(LOG_ERR, "%s", "Test for log!");

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
    while ((serverId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL |
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

    serverIdFd = open(SERVER_KEY_FILE, O_WRONLY | O_CREAT, S_IRGRP | S_IWUSR | S_IRUSR);
    if (serverIdFd == -1) {
        fprintf(stderr, "open server System V IPC Message Queue ID file failed! Path is %s, line = %d\n",
                SERVER_KEY_FILE, __LINE__);
        exit(EXIT_FAILURE);
    }

    if (write(serverIdFd, (void *)&serverId, sizeof(serverId)) != sizeof(serverId)) {
        fprintf(stderr, "wite server System V IPC Message Queue ID file failed! Path is %s, line = %d\n",
                SERVER_KEY_FILE, __LINE__);
        close(serverIdFd);
        remove(SERVER_KEY_FILE);
        exit(EXIT_FAILURE);
    }

    close(serverIdFd);

    if (atexit(onExit) == -1) {
        fprintf(stderr, "atexit failed! line = %d\n", __LINE__);
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