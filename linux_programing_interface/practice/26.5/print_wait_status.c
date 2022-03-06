#define _GNU_SOURCE
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include "print_wait_status.h"

void printWaitStatus(const char *msg, int status)
{
    if (msg != NULL) {
        printf("%s\n", msg);
    }

    if (WIFEXITED(status)) {
        printf("child exit, status=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("child killed by signal %d (%s)", WTERMSIG(status), strsignal(WTERMSIG(status)));

#ifdef WCOREDUMP
        if (WCOREDUMP(status)) {
            printf("core dumped");
        }
#endif
        printf("\n");
    } else if (WIFSTOPPED(status)) {
        printf("child stopped by signal %d (%s)\n", WSTOPSIG(status), strsignal(WSTOPSIG(status)));
    } else if (WIFCONTINUED(status)) {
        printf("child continued\n");
    } else {
        printf("what happend to this child? (status=%x)\n", status);
    }
}