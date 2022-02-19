#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <myDef.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "id_echo.h"

#define ERROUT(...) errPrintf(NULL, __LINE__, strerror(errno), __VA_ARGS__)

static void sigchildHandler (int signal)
{
    int saveErrno;

    saveErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }

    errno = saveErrno;
}

static void handRequest(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t numRd;

    while ((numRd = read(cfd, (void *)buf, sizeof(buf))) > 0) {
        if (write(cfd, (void *)buf, numRd) != numRd) {
            syslog(LOG_ERR, "write() failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (numRd == -1) {
        syslog(LOG_ERR, "Error from read failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int main(char argc, char *argv[])
{
    struct sigaction sa;
    int lfd, cfd;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigchildHandler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        ERROUT("sigaction failed!");
    }

    lfd = inetListen(SERVICE, 10, NULL);
    if (lfd == -1) {
        ERROUT("inetListen failed!");
    }

    while (1) {
        cfd = accept(lfd, NULL, NULL);
        if (cfd == -1) {
            syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
        }

        switch (fork()) {
            case -1:
                syslog(LOG_ERR, "Can't create child (%s)", strerror(errno));
                close(cfd);
                break;
            case 0:
                close(lfd); // 子进程关闭父进程的服务器socket
                handRequest(cfd);
                _exit(EXIT_SUCCESS);
                break;
            default:
                close(cfd);
                break;
        }
    }
}