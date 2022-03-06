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

int shmId, semId;

static int mutexInit(int semId, int type)
{
    union semun arg;
    int semNum;

    switch (type) {
        case MUTEX_WRITE:
            arg.val = 1;
            semNum = 1;
            break;
        case MUTEX_READ:
            arg.val = 0;
            semNum = 0;
            break;
        default:
            return -1;
    }

    return semctl(semId, semNum, SETVAL, arg);
}

static void onExit(void)
{
    shmctl(shmId, IPC_RMID, NULL);
    semctl(semId, 0, IPC_RMID, NULL);
}

int main(char argc, char *argv[])
{
    int fd;
    struct shm *shmp = NULL;
    int WriteNum = 0;

    if ((argc != 2) || (strcmp(argv[1], "--help") == 0)) {
        ERROUT("Usage: %s PathName ", argv[0]);
    }

    atexit(onExit);

    while ((shmId = shmget(SHM_KEY, sizeof(struct shm), IPC_CREAT | IPC_EXCL | XFR_PERMISSION)) == -1) {
        if (errno == EEXIST) {
            shmId = shmget(SHM_KEY, 0, 0);
            if (shmId != -1) {
                shmctl(shmId, IPC_RMID, NULL);
                continue;
            }
        }
        ERROUT("sgmget failed!");
    }

    //shmId = shmget(SHM_KEY, sizeof(struct shm), IPC_CREAT | IPC_EXCL | XFR_PERMISSION);
    //if (shmId == -1) {
    //    ERROUT("shmget failed!");
    //}

    shmp = shmat(shmId, NULL, 0);
    if (shmp == (void *)-1) {
        ERROUT("shmat failed!");
    }

    semId == semget(SEM_KEY, 2, IPC_CREAT | IPC_EXCL | XFR_PERMISSION);
    if (semId == -1) {
        ERROUT("semget failed!");
    }

    if (mutexInit(semId, MUTEX_READ) == -1) {
        ERROUT("semctl Read failed!");
    }

    if (mutexInit(semId, MUTEX_WRITE) == -1) {
        ERROUT("semctl Write failed!");
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        ERROUT("read %s failed!");
    }

    while (1) {
        if (mutextGet(semId, MUTEX_WRITE) == -1) {
            ERROUT("sem get Write failed!");
        }

        shmp->length = read(fd, (void *)shmp->msg, BUF_SIZE);
        if (shmp->length == 0) {
            printf("Write: read end of failed!\n");
            break;
        }
        WriteNum += shmp->length;

        if (mutexPut(semId, MUTEX_READ) == -1) {
            ERROUT("sem release Read failed!");
        }
    }

    close(fd);

    /*
     * notice read process
     * now read end of file
     */
    if (mutexPut(semId, MUTEX_READ) == -1) {
        ERROUT("sem release Read failed!");
    }

    printf("Write: %d \n", WriteNum);

    shmdt(shmp);
    shmctl(shmId, IPC_RMID, NULL);
    semctl(semId, 0, IPC_RMID, NULL);

    exit(EXIT_SUCCESS);
}