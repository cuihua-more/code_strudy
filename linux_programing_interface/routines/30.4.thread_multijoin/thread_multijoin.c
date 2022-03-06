#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

enum status {
    TS_ALIVE,
    TS_TERMINATED,
    TS_JOINED,
};

struct thread_info {
    pthread_t id;
    enum status status;
    int sleepTimes;
};

struct thread_info *threads = NULL;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int numNoJoined = 0;

static void * threadFunc(void *param)
{
    int id = *((int *)param);
    int s;

    sleep(threads[id].sleepTimes);
    printf("Thread %d has sleeped %d s.", id, threads[id].sleepTimes);

    s = pthread_mutex_lock(&mutex);
    if (s != 0) {
        fprintf(stderr, "pthread_mutex_lock failed! line=%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }
    
    numNoJoined++;
    threads[id].status = TS_TERMINATED;

    s = pthread_mutex_unlock(&mutex);
    if (s != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed! line=%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    s = pthread_cond_signal(&cond);
    if (s != 0) {
        fprintf(stderr, "pthread_cond_signal failed! line=%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    return NULL;
}

int main(char argc, char *argv[])
{
    int i, s;
    int numLive = 0;

    if ((argc > 2) && (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s nescs...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    threads = calloc(argc - 1, sizeof(struct thread_info));
    if (threads == NULL) {
        fprintf(stderr, "calloc error! line=%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < (argc - 1); i++) {
        threads[i].sleepTimes = (int)atol(argv[i + 1]);
        threads[i].status = TS_ALIVE;

        if (pthread_create(&threads[i].id, NULL, threadFunc, &i) != 0) {
            fprintf(stderr, "pthread_create failed! line=%d\n", __LINE__);
            exit(EXIT_FAILURE);
        }
    }

    numLive = argc - 1;

    while (numLive > 0) {
        s = pthread_mutex_lock(&mutex);
        if (s != 0) {
            fprintf(stderr, "pthread_mutex_lock failed! line=%d\n", __LINE__);
            exit(EXIT_FAILURE);
        }

        while (numNoJoined == 0) {
            s =  pthread_cond_wait(&cond, &mutex);
            if (s != 0) {
                fprintf(stderr, "pthread_cond_wait failed! line=%d\n", __LINE__);
                exit(EXIT_FAILURE);
            }
        }
    }

    for (i = 0; i < argc - 1; i++) {
        if (threads[i].status == TS_TERMINATED) {
            s = pthread_join(threads[i].id, NULL);
            if (s != 0) {
                fprintf(stderr, "pthread join failed! line=%d\n", __LINE__);
                exit(EXIT_FAILURE);
            }

            threads[i].status = TS_JOINED;
            numNoJoined--;
            numLive--;

            printf("Reaped thread %d (numLive=%d)\n", i, numLive);
        }

    }

    s = pthread_mutex_unlock(&mutex);
    if (s != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed! line=%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}