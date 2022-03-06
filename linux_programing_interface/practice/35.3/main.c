#define _GNU_SOURCE
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <sys/times.h>

#define SCHED_PRIO  20

static void sehedFuncErrPrintf(const char *func, int err, int line)
{
    fprintf(stderr, "%s failed! errno = %d line = %d\n", func, err, line);
    switch (err) {
        case EPERM:
            fprintf(stderr, "The calling thread dose not have appropriate privileges.\r\n");
            break;
        
        case ESRCH:
            fprintf(stderr, "The thread whose ID is pid could not be found.\r\n");
            break;
        
        case EINVAL:
            fprintf(stderr, "param does not make sense for the specified policy.\r\n");
            break;
        
        default:
            break;    
    }
    exit(EXIT_FAILURE);
}

static void delay3s(void)
{
    struct tms tms;
    int timebase = sysconf(_SC_CLK_TCK);
    char count;
    int oldTimes;

    timebase /= 4;

    printf("PID = %ld, timeBase = %d\r\n", (long)getpid(), timebase);

    count = 0;
    oldTimes = 0;

    alarm(5);
    while (1) {
        if (times(&tms) == -1) {
            fprintf(stderr, "PID = %ld, times() failed! line = %d \r\n",
                    (long)getpid(),
                    __LINE__);
            exit(EXIT_FAILURE);
        }

        if ((tms.tms_stime + tms.tms_utime) != oldTimes) {
            oldTimes = (tms.tms_stime + tms.tms_utime);
            if (((tms.tms_stime + tms.tms_utime) % timebase) == 0) {
                count++;
                printf("PID = %ld custom %.2fs, CPU time = %ld\n",
                       (long)getpid(),
                       (float)0.25 * count,
                       (long)(tms.tms_stime + tms.tms_utime));
                if (count == 12) {
                    printf("PID = %ld has been 3s\n", (long)getpid());
                    break;
                }
                if ((count % 4) == 0) {
                    sched_yield();
                }
            }
        }
        
    }
}

int main(char argc, char *argv[])
{
    struct sched_param param;

    param.sched_priority = sched_get_priority_min(SCHED_FIFO) + SCHED_PRIO;

    errno = 0;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1)
    {
        sehedFuncErrPrintf("sched_setscheduler", errno, __LINE__);
    }

    switch (fork()) {
        case -1:
            fprintf(stderr, "fork failed! line = %d\r\n", __LINE__);
            exit(EXIT_FAILURE);
        case 0:
            /* child */
            delay3s();
            break;

        default:
            /* father */
            delay3s();
            break;
        }

     exit(EXIT_SUCCESS);
}
