#define _GNU_SOURCE
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include "print_wait_status.h"

static volatile int numLiveChildren = 0;

static void sigchldHandler(int sig)
{
    int status, saveERRNO;
    pid_t childPid;

    saveERRNO = errno;

    printf("handler: Caught SIGCHLD\n");
    while ( (childPid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("handler: Repead child %ld - ", (long)childPid);
        printWaitStatus(NULL, status);
        numLiveChildren--;
    }

    if (childPid == -1 && errno != ECHILD) {
        fprintf(stderr, "waitpid failed! line:%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    sleep(5);
    printf("handler: returning\n");

    errno = saveERRNO;
}

int main(char argc, char *argv[])
{
    int i, signum;
    sigset_t blockMask, emptyMask;
    struct sigaction sa;

    if ((argc > 1) && (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s child-sleep-time...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    setbuf(stdout, NULL);

    signum = 0;
    numLiveChildren = argc - 1;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigchldHandler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction failed! line:%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);
    if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1) {
        fprintf(stderr, "sigprocmask failed! line:%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < argc; i++) {
        switch (fork()) {
            case -1:
                fprintf(stderr, "fork failed! line:%d\n", __LINE__);
                exit(EXIT_FAILURE);
            case 0:
                sleep((unsigned int)atol(argv[i]));
                printf("Child %d (PID=%ld) exiting\n", i, (long)getpid());
                _exit(EXIT_SUCCESS);
            default :
                break;
        }
    }

    sigemptyset(&emptyMask);
    while (numLiveChildren > 0) {
        if ((sigsuspend(&emptyMask) == -1) && (errno != EINTR)) {
            fprintf(stderr, "sigsuspend failed! line:%d\n", __LINE__);
            exit(EXIT_FAILURE);
        }
        signum++;
    }

    printf("All %d children have terminated; SIGCHLD was caught: %d times\n", argc - 1, signum);

    exit(EXIT_SUCCESS);
}