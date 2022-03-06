#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct oneTime {
    char once;
    pthread_mutex_t mutex;
};

#define ONE_TIME_INITIALIZER  {             \
    .once = 0,                              \
    .mutex = PTHREAD_MUTEX_INITIALIZER,     \
}

struct oneTime oneTime = ONE_TIME_INITIALIZER;

int one_time_init(struct oneTime *once, void (*init)(void))
{
    pthread_mutex_lock(&once->mutex);
    if (once->once) {
        pthread_mutex_unlock(&once->mutex);
        return -1;
    } 

    init();
    once->once = 1;
    pthread_mutex_unlock(&once->mutex);
    return 0;
}

static void onceFunc(void)
{
    static int time = 0;

    time++;
    printf("call onceFunc %d times\n", time);
}

int main(char argc, char *argv[])
{

    one_time_init(&oneTime, onceFunc);
    one_time_init(&oneTime, onceFunc);
    exit(EXIT_SUCCESS);
}
