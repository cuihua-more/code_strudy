#define _GNU_SOURCE
#include "stdio.h"
#include "signal.h"
#include "stdlib.h"
#include "string.h"
#include <sys/types.h>
#include <unistd.h>

static void handler(int sig)
{
    printf("PID %ld caught signal %d (%s)\n", (long)getpid(), sig, strsignal(sig));
}

int main(char argc, char *argv[])
{

    pid_t parentPid, childPid;
    int i;
    struct sigaction sa;

    if ((argc < 2) || (strcmp(argv[1], "---help") == 0)) {
        /*
         * 执行exec命令会将当前终端的控制程序由shell替换为当前要执行的程序
         */
        fprintf(stderr, "exec %s {s|d}... [ > sig.log 2>&1]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    setbuf(stdout, NULL);

    parentPid = getpid();
    printf("PID of parent proces is         %ld\n", (long)parentPid);
    printf("Foreground process group ID is: %ld\n", (long)tcgetpgrp(STDIN_FILENO));

    for (i = 1; i < argc; i++) {
        childPid = fork();
        if (childPid == -1 ) {
            fprintf(stderr, "fork failed! \n");
            exit(EXIT_FAILURE);
        }

        if (childPid == 0) {
            if (argv[i][0] == 'd') {
                if (setpgid(0, 0) == -1) {
                    fprintf(stderr, "setggid failed! \n");
                    exit(EXIT_FAILURE);
                } 
            }
            sa.sa_flags = 0;
            sigemptyset(&sa.sa_mask);
            sa.sa_handler = handler;
            if (sigaction(SIGHUP, &sa, NULL) == -1) {
                fprintf(stderr, "sigaction failed! \n");
                exit(EXIT_FAILURE);
            }
            break;
        } 
    }
    alarm(60);

    printf("PID = %ld PGID = %ld\n", (long)getpid(), (long)getgid());

    for (;;) {
        pause();
    }

    exit(EXIT_SUCCESS);
}