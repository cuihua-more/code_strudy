#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

static void printfUsage(char *str)
{
    struct rusage usage;

    if (getrusage(RUSAGE_CHILDREN, &usage) == -1) {
        fprintf(stderr, "getrusage failed! line = %d\t\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("%s\r\n", str);

    printf("User CPU time used: Sec is %ld, uSec = %ld \n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
    printf("System CPU time used: Sec is %ld, uSec = %ld \n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
    printf("Max of resident set: %ld KB\r\n", usage.ru_maxrss);
    printf("Integral (shared) text memnor size: %ld KB\n", usage.ru_ixrss);
    printf("Integral (unshared) data memnor size: %ld KB\n", usage.ru_idrss);
    printf("Integral (unshared) stack memnor used: %ld KB\n", usage.ru_isrss);
    printf("Soft page faults (I/O not required): %ld\n", usage.ru_minflt);
    printf("Hard page faults (I/O required): %ld\n", usage.ru_majflt);
    printf("Swaps out of physical memory [unuesd]: %ld\n", usage.ru_nswap);
    printf("Block input operations via file system: %ld\n", usage.ru_inblock);
    printf("Block output operations via file system: %ld\n", usage.ru_oublock);
    printf("IPC messages send: %ld\n", usage.ru_msgsnd);
    printf("IPC messages received: %ld\n", usage.ru_msgrcv);
    printf("Voluntary  context switched (process relinquished CPU before its time slice expired): %ld\n", usage.ru_nvcsw);
    printf("Voluntary  context switched (highter priority process became runnable o time slice ran out): %ld\n", usage.ru_nivcsw);
}

int main(char argc, char *argv[])
{

    switch (fork()) {
        case -1:
            fprintf(stderr, "fork failed!\r\n");
            break;

        case 0:
            alarm(3);
            int i;
            while (1) {
                i++;
            }
            exit(EXIT_SUCCESS);
        default:
            printfUsage("Before:");
            wait(NULL);
            printfUsage("After:");
            break;
    }

    exit(EXIT_SUCCESS);
}