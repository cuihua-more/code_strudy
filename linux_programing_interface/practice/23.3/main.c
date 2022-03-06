
#define _GUN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>

static void handler(int sig, siginfo_t *info, void *uc)
{
    printf("After timer\n");
    printf("    receive signal is %ld \n    timer ID is %d\n", (long)info->si_signo, info->si_value.sival_int);
}

int main(char argc, char *argv[])
{
    timer_t tmr;
    struct itimerspec ts;
    struct sigaction action;

    if (timer_create(CLOCK_REALTIME,NULL, &tmr) == -1 ) {
        fprintf(stderr, "%s: timer_creat failed! line is %d\n", argv[0], __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("Before timer, timer ID is %d\n", (int)tmr);

    ts.it_value.tv_sec = 1;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    printf("Start Timer\n");
    if (timer_settime(tmr, 0, &ts, NULL) == -1) {
        fprintf(stderr, "%s: timer_settime failed! line is %d\n", argv[0], __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("Sleep 1 s\n");

    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;

    if (sigaction(SIGALRM, &action, NULL) == -1) {
        fprintf(stderr, "%s: sigaction failed! line is %d\n", argv[0], __LINE__);
        exit(EXIT_FAILURE);
    }

    while (1) {
        pause();
    }

    exit(EXIT_SUCCESS);
}