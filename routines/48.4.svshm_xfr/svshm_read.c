#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "svshm_xfr.h"
#include <errno.h>
#include "svmutex.h"



int main(char argc, char *argv[])
{
    int semId, shmId;
    struct shm *shmp = NULL;
    int ReadNum = 0;

    shmId = shmget(SHM_KEY, 0, 0);
    if (shmId == -1) {
        ERROUT("shmget failed!");
    }

    shmp = shmat(shmId, NULL, SHM_RDONLY);
    if (shmp == (void *)-1) {
        ERROUT("shmat failed!");
    }

    semId = semget(SEM_KEY, 0, 0);
    if (semId == -1) {
        ERROUT("semget failed!");
    }

    while (1) {
        if (mutextGet(semId, MUTEX_READ) == -1) {
            ERROUT("mutex Read get failed!");
        }

        if (shmp->length == 0) {
            break;
        }
        ReadNum += shmp->length;

        write(STDOUT_FILENO, (void *)shmp->msg, shmp->length);

        if (mutexPut(semId, MUTEX_WRITE) == -1) {
            ERROUT("mtex Write put failed!");
        }
    }

    fprintf(stderr, "Read: %d \n", ReadNum);
    exit(EXIT_SUCCESS);
}