#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>

struct file_buf {
    int len;
    char buf[1024];
};

struct file_buf Gbuf;
sem_t semWrite, semRead;

static void errPrintf(int line, char *fmt, ...)
{
    char buf[4096];
    va_list ap;

    memset((void *)buf, 0, sizeof(buf));
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " line = %d ", line);
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " %s \n ", strerror(errno));

    write(STDERR_FILENO, (void *)buf, strlen(buf));
}
#define ERROUT(...) errPrintf(__LINE__, ##__VA_ARGS__)

static void *thread_write(void *args)
{
    int ret = 0;
    int fd = *((int *)args);
    int num = 0;

    while (1) {
        if (sem_wait(&semRead) == -1) {
            if (errno == EINTR) {
                continue;
            }
            ERROUT("sem_wait read failed!");
            ret = -1;
        }
        num += Gbuf.len;
        if (Gbuf.len == 0) {
            break;
        }
        write(fd, (void *)Gbuf.buf, Gbuf.len);
        sem_post(&semWrite);
    }

    printf("Write %d Bytes \n", num);
    pthread_exit(&ret);
}

int main(char argc, char *argv[])
{
    int fdRead, fdWrite;
    char *pStr = NULL;
    int ret = EXIT_FAILURE;
    pthread_t threadWrite;

    if ((argc != 3) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s file_src file_dest \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (sem_init(&semWrite, 0, 1) == -1){
        ERROUT("sem_init Write failed!");
        goto err_semWrite;
    }

    if (sem_init(&semRead, 0, 0) == -1){
        ERROUT("sem_init Read failed!");
        goto err_semRead;
    }

    fdRead = open(argv[1], O_RDONLY);
    if (fdRead == -1) {
        ERROUT("open %s file failed!", argv[1]);
        goto err_openRead;
    }

    fdWrite = open(argv[2], O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);
    if (fdWrite == -1) {
        ERROUT("create %s file filed!", argv[2]);
        goto err_createWrite;
    }

    if (pthread_create(&threadWrite, NULL, thread_write, &fdWrite) != 0) {
        ERROUT("pthread_create failed!");
        goto err_thread;
    }

    while (1) {
        if (sem_wait(&semWrite) == -1) {
            if (errno == EINTR) {
                continue;
            }
            Gbuf.len = 0;
            sem_post(&semRead);
            ERROUT("sem_wait Write failed!");
            break;
        }

        Gbuf.len = read(fdRead, (void *)Gbuf.buf, sizeof(Gbuf.buf));
        if (Gbuf.len == 0) {
            sem_post(&semRead);
            break;
        }
        sem_post(&semRead);
    }

    pthread_join(threadWrite, NULL);
    printf("thrad stop\n");

    ret = EXIT_SUCCESS;

err_thread:
    close(fdWrite);
err_createWrite:
    close(fdRead);
err_openRead:
    sem_destroy(&semRead);
err_semRead:
    sem_destroy(&semWrite);
err_semWrite:
    exit(ret);
}