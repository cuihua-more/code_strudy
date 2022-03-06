#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <myDef.h>
#include "common.h"
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ERROUT(...) errPrintf(__FILE__, __LINE__, strerror(errno), __VA_ARGS__)

static void sigchildHandler (int sig)
{
    int saveErrno;

    saveErrno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }

    errno = saveErrno;
}

static void data(int cfd)
{
    char buf[BUF_SZIE];
    ssize_t numRd;

    /* 重定向STDIN为套接字 */
    close(STDIN_FILENO);
    if (dup(cfd) != STDIN_FILENO) {
        //if (dup2(cfd, STDIN_FILENO) == -1) {
        //    ERROUT("child PID is %ld dup failed!\n", (long)getpid());
        //}
        while (dup2(cfd, STDIN_FILENO) == -1) {
            continue;
        }
    }

    /* 重定向STDOUT为套接字描述符 */
    close(STDOUT_FILENO);
    if (dup(cfd) != STDOUT_FILENO) {
        //if (dup2(cfd, STDOUT_FILENO) == -1) {
        //    ERROUT("child PID is %ld dup failed!\n", (long)getpid());
        //}
        while (dup2(cfd, STDOUT_FILENO) == -1) {
            continue;
        }
    }

    memset((void *)buf, 0, sizeof(buf));
    numRd = read(STDIN_FILENO, (void *)buf, sizeof(buf));
    if (numRd > 0) {
        system(buf);
    }
}

int main(char argc, char *argv[])
{
    int lfd, cfd;
    socklen_t addrLen;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigchildHandler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        ERROUT("sigaction failed!");
    }

    lfd = inetListen(SERVER, 10, &addrLen);
    if (lfd == -1) {
        ERROUT("inetListen failed!");
    }

    while (1) {
        cfd = accept(lfd, NULL, NULL);
        if (cfd == -1) {
            ERROUT("accept failed!");
        }
        switch (fork()) {
            case -1:
                ERROUT("fork failed!");
                break;
            case 0:
                /* child process */
                close(lfd);
                data(cfd);
                _exit(EXIT_FAILURE);
            default:
                close(cfd);
                break;
        }
        
    }
    

    exit(EXIT_SUCCESS);
}