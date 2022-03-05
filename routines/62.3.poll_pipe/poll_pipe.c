#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <myDef.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <time.h>

#define ERROUT(...) errPrintf(NULL, __LINE__, strerror(errno), __VA_ARGS__)

int main(char argc, char *argv[])
{
    int numPipes, writePipes;
    int (*pfds)[2] = NULL;
    int i;
    int randPipe;
    struct pollfd *pollFd = NULL;
    int ready;

    if ((argc != 3) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s num-pipes [num-writes]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    numPipes = (int)atol(argv[1]);
    writePipes = (int)atol(argv[2]);

    pfds = calloc(numPipes, sizeof(int[2]));
    if (pfds == NULL) {
        ERROUT("calloc %d bytes failed!\n", numPipes * sizeof(int[2]));
    }

    pollFd = (struct pollfd *)calloc(numPipes, sizeof(struct pollfd));
    if (pollFd == NULL) {
        ERROUT("calloc %d bytes failed!\n", numPipes * sizeof(struct pollfd));
    }

    for (i = 0; i < numPipes; i++) {
        if (pipe(pfds[i])) {
            ERROUT("pipe %d times failed!\n", i);
        }
    }

    srandom((int)time(NULL));

    for (i = 0; i < writePipes; i++) {
        randPipe = random() % numPipes;
        printf("Writing to fd: %3d (read fd: %3d) \n", pfds[randPipe][1], pfds[randPipe][0]);
        if (write(pfds[randPipe][1], "a", 1) != 1) {
            ERROUT("write %d failed!", pfds[randPipe][1]);
        }
    }

    for (i = 0; i < numPipes; i++) {
        pollFd[i].fd = pfds[i][0];
        pollFd[i].events = POLLIN;
    }

    ready = poll(pollFd, numPipes, 0);
    if (ready == -1) {
        ERROUT("poll failed!");
    }

    printf("poll() returnedL %d\n", ready);

    for (i = 0; i < numPipes; i++) {
        if (pollFd[i].revents & POLLIN) {
            printf("Readable: %d %3d\n", i, pollFd[i].fd);
        }
    }

    exit(EXIT_SUCCESS);
}