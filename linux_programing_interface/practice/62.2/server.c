#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <myDef.h>
#include "common.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>

#define ERROUT(...) errPrintf(__FILE__, __LINE__, strerror(errno), __VA_ARGS__)

static void tcpHandle(int fd)
{
    int lfd;
    ssize_t numRd;
    char buf[BUF_SIZE];

    lfd = accept(fd, NULL, NULL);
    if (lfd == -1) {
        ERROUT("accept failed!");
    }

    numRd = read(lfd, (void *)buf, sizeof(buf));
    if (numRd < 0) {
        ERROUT("read failed!");
    }
    write(lfd, (void *)buf, numRd);
}

static void udpHandle(int fd)
{
    char buf[BUF_SIZE];
    ssize_t numRd;
    struct sockaddr sockaddr;
    socklen_t len;

    len = sizeof(sockaddr);
    numRd = recvfrom(fd, (void *)buf, sizeof(buf), 0, &sockaddr, &len);
    if (numRd < 0) {
        ERROUT("read failed!");
    }

    if (sendto(fd, (void *)buf, numRd, 0, &sockaddr, len) != numRd) {
        ERROUT("write failed!");
    }
}

int pollServer(int tcpFd, int udpFd)
{
    struct pollfd readPoll[3];
    int ready;
    int i;

    readPoll[0].fd = tcpFd;
    readPoll[0].events = POLLIN;
    readPoll[1].fd = tcpFd;
    readPoll[1].events = POLLHUP;
    readPoll[2].fd = udpFd;
    readPoll[2].events = POLLIN;

    while (1) {
        ready = poll(readPoll, sizeof(readPoll) / sizeof(struct pollfd), 10);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                ERROUT("poll failed!");
            }
        }
        for (i = 0; i < sizeof(readPoll) / sizeof(struct pollfd); i++) {
            if (readPoll[i].revents & POLLIN) {
                switch (fork()) {
                case -1:
                    ERROUT("fork failed!");
                    break;
                case 0:
                    readPoll[i].fd == tcpFd ? tcpHandle(readPoll[i].fd) : udpHandle(readPoll[i].fd);
                    _Exit(EXIT_SUCCESS);

                default:
                    break;
                }
            } else if (readPoll[i].revents & POLLHUP) {
                printf("fd: %d recv POLLHUP\n", readPoll[i].fd);
            }
        }
    }

    exit(EXIT_SUCCESS);
}

int selectServer(int tcpFd, int udpFd)
{
    int nfds;
    fd_set readFds;
    int ready;
    struct timeval tm;

    nfds = tcpFd > udpFd ? tcpFd + 1 : udpFd + 1;

    while (1) {
        FD_ZERO(&readFds);
        FD_SET(tcpFd, &readFds);
        FD_SET(udpFd, &readFds);
        tm.tv_sec = 0;
        tm.tv_usec = 1000;
        ready = select(nfds, &readFds, NULL, NULL, &tm);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                ERROUT("select");
            }
        } else if (ready == 0) {
            continue;
        } else {
            if (FD_ISSET(tcpFd, &readFds)) {
            tcpHandle(tcpFd);
            } else if (FD_ISSET(udpFd, &readFds)) {
                udpHandle(udpFd);
            } else {
                ERROUT("Ready fd isn't tcpFd or udpFd");
            }
        }
    }
}

int epollServer(int tcpFd, int udpFd)
{
    int eFd;
    struct epoll_event eEvent;
    struct epoll_event evList[2];
    int ready;

    eFd = epoll_create(10);
    if (eFd == -1) {
        ERROUT("epoll_create failed!");
    }

    eEvent.data.fd = tcpFd;
    eEvent.events = EPOLLIN;
    if (epoll_ctl(eFd, EPOLL_CTL_ADD, tcpFd, &eEvent)) {
        ERROUT("epoll_ctl add tcpFd event failed!");
    }
    eEvent.data.fd = udpFd;
    eEvent.events = EPOLLIN;
    if (epoll_ctl(eFd, EPOLL_CTL_ADD, udpFd, &eEvent)) {
        ERROUT("epoll_ctl add udpFd event failed!");
    }

    while (1) {
        ready = epoll_wait(eFd, evList, sizeof(evList) / sizeof(struct epoll_event), 10);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                ERROUT("epoll_wait failed!");
            }
        } else if (ready == 0) {
            continue;
        } else {
            for (int i = 0; i < ready; i++) {
                if (evList[i].events & EPOLLIN) {
                    evList[i].data.fd == tcpFd ? tcpHandle(evList[i].data.fd) : udpHandle(evList[i].data.fd);
                }
            }
        }
    }
}

static void sigusr1Handler(int sig, siginfo_t *info, void *ucontext)
{
    switch(fork()) {
        case -1:
            ERROUT("fork failed!");
            break;
        case 0:
            tcpHandle(info->si_fd);
            _exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }
}

static void sigusr2Handler(int sig, siginfo_t *info, void *ucontext)
{
    switch(fork()) {
        case -1:
            ERROUT("fork failed!");
            break;
        case 0:
            udpHandle(info->si_fd);
            _exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }
}

int signalServer(int tcpFd, int udpFd)
{
    int flags;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = sigusr1Handler;
    if (sigaction(SIGUSR1, &sa, NULL)) {
        ERROUT("sigacrion SIGUSR1 failed!");
    }
    sa.sa_sigaction = sigusr2Handler;
    if (sigaction(SIGUSR2, &sa, NULL)) {
        ERROUT("sigacrion SIGUSR2 failed!");
    }

    if (fcntl(tcpFd, F_SETSIG, SIGUSR1)) {
        ERROUT("fcntl tcpFd set SIGUSR1 failed!");
    }

    if (fcntl(udpFd, F_SETSIG, SIGUSR2)) {
        ERROUT("fcntl udpFd set SIGUSR1 failed!");
    }

    flags = fcntl(tcpFd, F_GETFL);
    fcntl(tcpFd, F_SETFL, flags | O_ASYNC | O_NONBLOCK);
    
    flags = fcntl(udpFd, F_GETFL);
    fcntl(udpFd, F_SETFL, flags | O_ASYNC | O_NONBLOCK);

    if (fcntl(tcpFd, F_SETOWN, getpid())) {
        ERROUT("fcntl tcpFd OWN failed!");
    }

    if (fcntl(udpFd, F_SETOWN, getpid())) {
        ERROUT("fcntl udpFd OWN failed!");
    }

    while (1) {
        waitpid(-1, NULL, WNOHANG);
    }
}

int main(char argc, char *argv[])
{
    int tcpFd, udpFd;

    if ((argc != 2) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s poll/select/epoll/signal\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    tcpFd = inetListen(SERVER, 10, NULL);
    if (tcpFd == -1) {
        ERROUT("inetListen failed!");
    }

    udpFd = inetBind(SERVER, SOCK_DGRAM, NULL);
    if (udpFd == -1) {
        ERROUT("inetBind failed!");
    }

    if (strcmp(argv[1], "poll") == 0) {
        pollServer(tcpFd, udpFd);
    } else if (strcmp(argv[1], "select") == 0) {
        selectServer(tcpFd, udpFd);
    } else if (strcmp(argv[1], "epoll") == 0) {
        epollServer(tcpFd, udpFd);
    } else if (strcmp(argv[1], "signal") == 0) {
        signalServer(tcpFd, udpFd);
    } else {
        fprintf(stderr, "Usage: %s poll/select/epoll/signal\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}