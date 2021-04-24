#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdlib.h>

static void displayProcessTimes(const char *msg)
{
    static long timeBase;
    clock_t clockTime;
    struct tms t;

    if (msg != NULL) {
        printf("%s", msg);
    }

    if (timeBase == 0) {
        timeBase = sysconf(_SC_CLK_TCK);
        if (timeBase == -1) {
            fprintf(stderr, "get timeBase failed! line = %d \n", __LINE__);
            exit(EXIT_FAILURE);
        }
    }

    clockTime = clock();
    if (clockTime == -1) {
        fprintf(stderr, "clock() failed! line = %d \n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("clock() return: %ld clock-per-sec (%.2f secs)\n",
           (long)clockTime, (double)clockTime / CLOCKS_PER_SEC);
        
    if (times(&t) == -1) {
        fprintf(stderr, "times() failed! line = %d \n", __LINE__);
        exit(EXIT_FAILURE);
    }
    printf("times() yields: user CPU: %.2f; system CPU: %.2f\n",
           (double)t.tms_utime / timeBase,
           (double)t.tms_stime / timeBase);
}

int main(char argc, char *argv[])
{
    int numCalls, i;

    printf("CLOCKS_PER_SEC=%ld  sysconf(_SC_CLK_TCK)=%ld\n\n",
           (long)CLOCKS_PER_SEC, sysconf(_SC_CLK_TCK));

    displayProcessTimes("At program start:\n");

    numCalls = (argc > 1) ? atol(argv[1]) : 100000;

    for (i = 0; i < numCalls; i++) {
        (void)getppid();
    }

    displayProcessTimes("After gitppid() loop: \n");
}