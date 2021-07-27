#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <signal.h>
#include <errno.h>

#define CMD_SIZE    200
#define SYNC_SIG    SIGUSR1

static void handler(int sig)
{

}

static void childReturnCallbaack(void)
{
    kill(getppid(), SYNC_SIG);
}

int main(char argc, char *argv[])
{
    char cmd[CMD_SIZE];
    pid_t childPid;
    sigset_t blockMask, emptyMask, originMask;
    struct sigaction sa;

    setbuf(stdout, NULL);

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SYNC_SIG);
    if (sigprocmask(SIG_BLOCK, &blockMask, &originMask) == -1) {
        fprintf(stderr, "sigprocmask failed! line: %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SYNC_SIG, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction failed! line: %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("Parenet Pid = %ld\n", (long)getpid());

    switch ((childPid = fork())) {
        case -1 :
            fprintf(stderr, "fork failed! line: %d\n", __LINE__);
            exit(EXIT_FAILURE);
        case 0:
            printf("Child (PID = %ld) exiting!\n", (long)getpid());
            atexit(childReturnCallbaack);
            exit(EXIT_SUCCESS);
        default :
            sigemptyset(&emptyMask);
            if ((sigsuspend(&emptyMask) == -1) && (errno != EINTR)) {
                fprintf(stderr, "sigsuspend failed! line: %d\n", __LINE__);
                exit(EXIT_FAILURE);
            }
            snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
            cmd[CMD_SIZE - 1] = '\0';
            system(cmd);

            if (kill(childPid, SIGKILL) == -1) {
                fprintf(stderr, "kill failed! line: %d\n", __LINE__);
                exit(EXIT_FAILURE);
            }
            sleep(3);
            printf("After sending SIGKILL to zombie (PID = %ld): \n", (long)childPid);
            system(cmd);
            exit(EXIT_SUCCESS);
    }
}