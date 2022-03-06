#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

int main(char argc, char *argv[])
{
    int maxind;
    struct shmid_ds ds;
    struct shminfo info;
    int ind;
    int id;

    maxind = shmctl(0, SHM_INFO, (struct shmid_ds *)&info);
    if (maxind == -1) {
        fprintf(stderr, "shmctl failed! line = %d %s", __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("maxind = %d\n", maxind);
    printf("index   id  key     quot\n");

    for (ind = 0; ind < maxind; ind++) {
        id = shmctl(ind, SHM_STAT, &ds);
        if (id == -1) {
            fprintf(stderr, "shmctl failed! line = %d %s", __LINE__, strerror(errno));
            continue;
        }
        printf("%d  %d  0x%08lX     %ld\n", ind, id,
               (unsigned long)ds.shm_perm.__key, ds.shm_nattch);
    }

    exit(EXIT_SUCCESS);
}