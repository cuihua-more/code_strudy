#define _GNU_SOURCE
#include "svmutex.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define ERROUT_FILENAME
#ifdef ERROUT_FILENAME
void errPrintf(int line, char *file, const char *fmt,...)
#else
void errPrintf(int line, const char *fmt,...)
#endif
{
    va_list ap;
    char buf[4096];

    memset((void *)buf, 0, sizeof(buf));
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

#ifdef ERROUT_FILENAME
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " File is %s", file);
#endif
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " line is %d ", line);
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " %s \n", strerror(errno));
    write(STDERR_FILENO, (void *)buf, strlen(buf));

    exit(EXIT_FAILURE);
}


int mutextGet(int semId, unsigned short type)
{
    struct sembuf op;

    op.sem_num = type;
    op.sem_op = -1;
    op.sem_flg = 0;

    return semop(semId, &op, 1);
}

int mutexPut(int semId, unsigned short type)
{
    struct sembuf op;

    op.sem_num = type;
    op.sem_op = 1;
    op.sem_flg = 0;
    
    return semop(semId, &op, 1);
}