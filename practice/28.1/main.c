#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static void *start(void *param)
{
    int ret;

    ret = pthread_join(pthread_self(), NULL);

    printf("After pthread_join self, return=%d\n", ret);
    pthread_exit(NULL);
}

int main(char argc, char *argv[])
{
    pthread_t ttid;

    if (pthread_create(&ttid, NULL, start, NULL) != 0) {
        fprintf(stderr, "pthread_create! line=%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("thread id = %ld\n", (long)ttid);

    exit(EXIT_SUCCESS);
}