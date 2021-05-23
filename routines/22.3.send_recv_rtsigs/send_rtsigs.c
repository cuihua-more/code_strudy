#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

int main(char argc, char *argv[])
{
    int numSigs;
    int i;
    union sigval sigval;

    if (argc < 2 || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s pid sig-num data [num-sigs]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%s: PID is %ld UID is %ld\n", argv[0], (long)getpid(), (long)getuid());

    numSigs = argc > 4 ? atol(argv[4]) : 1;

    for (i = 0; i < numSigs; i++) {
        sigval.sival_int = atol(argv[3]) + i;
        if (sigqueue(atol(argv[1]), atol(argv[2]), sigval) == -1) {
            fprintf(stderr, "%s: sigqueue failed! count is %d line:%d\n",
                    argv[0], i, __LINE__);
            exit(EXIT_FAILURE);
        }
    }

        exit(EXIT_SUCCESS);
}