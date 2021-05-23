#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static volatile int hanlerSleepTime;
static volatile int allDone = 0;
static volatile int sigCount = 0;

static void siginfoHandler(int sig, siginfo_t *info, void *ucontext)
{
    if ((sig == SIGINT) || (sig == SIGTERM)) {
        allDone = 1;
        return;
    }

    sigCount++;
    printf("caught signal %d\n", sigCount);

    printf("    si_signo=%d. si_code=%d (%s)", info->si_signo, info->si_code,
        (info->si_code == SI_QUEUE) ? "SI_QUEUE " : 
        (info->si_code == SI_USER)  ? "SI_USER " : "Other");
    printf("si_value=%d\n", info->si_value.sival_int);   
    printf("    si_pid=%d, si_uid=%d\n", info->si_pid, info->si_uid);
}

int main(char argc, char *argv[])
{
    int sig;
    struct sigaction action;
    sigset_t prevMask, blockMask;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [block-time [handler-sleep-time]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%s PID = %ld\n", argv[0], (long)getpid());

    hanlerSleepTime = argc > 2 ? atol(argv[2]) : 1;

    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART | SA_SIGINFO;
    action.sa_sigaction = siginfoHandler;

    for (sig = 0; sig < NSIG; sig++) {
        if ((sig == SIGKILL) && (sig == SIGSTOP)) {
            continue;
        }
        sigaction(sig, &action, NULL);
    }

    if (argc > 1) {
        sigfillset(&blockMask);
        sigdelset(&blockMask, SIGKILL);
        sigdelset(&blockMask, SIGSTOP);

        if (sigprocmask(SIG_SETMASK, &blockMask, &prevMask) == -1) {
            fprintf(stderr, "sigprocmask failed! line = %d\n", __LINE__);
            exit(EXIT_FAILURE);
        }

        printf("%s: signals blocks - sleeping %s seconds\n", argv[0], argv[1]);
        sleep(atol(argv[1]));
        printf("%s: sleep complete!\n", argv[0]);

        if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1) {
            fprintf(stderr, "sigprocmask failed! line = %d\n", __LINE__);
            exit(EXIT_FAILURE);
        }
    }

    while (!allDone) {
        pause();
    }

    exit(EXIT_SUCCESS);
}