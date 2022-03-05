#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <myDef.h>
#include <sys/select.h>
#include <time.h>

#define ERROUT(...) errPrintf(NULL, __LINE__, strerror(errno), __VA_ARGS__)

int main(char argc, char *argv[])
{
    int (*pfds)[2];
    int numPipes, writePipes;
    int i;
    int randPipes;
    int nfds;
    fd_set readSet;
    struct timeval tm;
    int ready, *readyfd;
    char tmp;

    if ((argc != 3) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s num-pipes [write-pipes]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    numPipes = (int)atol(argv[1]);
    writePipes = (int)atol(argv[2]);

    pfds = calloc(numPipes, sizeof(int[2]));
    if (pfds == NULL) {
        ERROUT("calloc %d failed!", numPipes * sizeof(int[2]));
    }

    readyfd = malloc(writePipes * sizeof(int));
    if (readyfd == NULL) {
        ERROUT("malloc %d bytes failed!", writePipes * sizeof(int));
    }

    for (i = 0; i < numPipes; i++) {
        if (pipe(pfds[i])) {
            ERROUT("pipe %d times failed!", i);
        }
    }

    srandom((int)time(NULL));
    for (i = 0; i < writePipes; i++) {
        randPipes = random() % numPipes;
        printf("Writing to fd: %3d (read fd: %3d)\n",
               pfds[randPipes][1], pfds[randPipes][0]);
        if (write(pfds[randPipes][1], "a", 1) != 1) {
            ERROUT("write %d failed!\n", pfds[randPipes][1]);
        }
        readyfd[i] = pfds[randPipes][0];
        if (nfds <= pfds[randPipes][0]) {
            nfds = pfds[randPipes][0] + 1;
        }
    }

    while (1) {
        FD_ZERO(&readSet);
        for (i = 0; i < writePipes; i++) {
            FD_SET(readyfd[i], &readSet);
        }
        tm.tv_sec = 1;
        tm.tv_usec = 1;
        ready = select(nfds, &readSet, NULL, NULL, &tm);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                ERROUT("select failed!");
            }
        } else if (ready == 0) {
            printf("Time out! readSet is emptied!\n");
            break;
        } else {
            printf("select() returned: %d\n", ready);
            for (i = 0; i < numPipes; i++) {
                if (FD_ISSET(pfds[i][0], &readSet)) {
                    printf("Readable: %d %3d\n", i, pfds[i][0]);
                    read(pfds[i][0], (void *)&tmp, 1);
                }
            }
        }
    }
    
    exit(EXIT_SUCCESS);
}