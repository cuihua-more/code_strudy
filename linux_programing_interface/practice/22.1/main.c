#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static void sigcontHandler(int sig)
{
    printf("SIGCONT signal!\n");
}

int main(char argc, char *argv[])
{
    sigset_t mask, prev;

    printf("%s PID is %ld\n", argv[0], (long)getpid());

    signal(SIGCONT, sigcontHandler);

    sigaddset(&mask, SIGCONT);

    if (sigprocmask(SIG_BLOCK, &mask, &prev) == -1) {
        fprintf(stderr, "sigprocmask failed! line is %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("%s: Please Stop cur Process, and then Revocer this Process!\n", argv[0]);
    printf("%s: enter sleep 20 sec\n", argv[0]);
    sleep(20);
    printf("%s: exit sleep\n", argv[0]);

    printf("%s: Before recover!\n", argv[0]);
    if (sigprocmask(SIG_SETMASK, &prev, NULL) == -1) {
        fprintf(stderr, "sigprocmask failed! line is %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("%s: After recover!\n", argv[0]);

    exit(EXIT_SUCCESS);
}