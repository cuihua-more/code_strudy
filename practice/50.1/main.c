#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>

int main(char argc, char *argv[])
{
    struct rlimit limit;
    int *p = NULL;
    int pageSize;

    if (getrlimit(RLIMIT_MEMLOCK, &limit) == -1) {
        fprintf(stderr, "getrlimit failed! line = %d \n", __LINE__);
        exit(EXIT_FAILURE);
    }

    if (limit.rlim_cur == RLIM_INFINITY) {
        printf("RLIMIT_MEMLOCK's cur is infinity! \n");
    }
    
    if (limit.rlim_max == RLIM_INFINITY) {
        printf("RLIMIT_MEMLOCK's cur is infinity! \n");
    }

    printf("Before: cur is %ld, max is %ld \n", limit.rlim_cur, limit.rlim_max);

    pageSize = sysconf(_SC_PAGE_SIZE);

    p = mmap(NULL, 2 * pageSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if (p == NULL) {
        fprintf(stderr, "mmap failed! \n");
        exit(EXIT_FAILURE);
    }

    if (mlock(p + 1, pageSize) == -1) {
        printf("mlock failed!\n");
    } else {
        printf("mlock success! \n");
    }

    munlock(p + 1, 2 * pageSize);

    limit.rlim_cur = pageSize;
    limit.rlim_max = pageSize;

    if (setrlimit(RLIMIT_MEMLOCK, &limit) == -1) {
        fprintf(stderr, "setrlimit failed! \n");
        exit(EXIT_FAILURE);
    }

    if (getrlimit(RLIMIT_MEMLOCK, &limit) == -1) {
        fprintf(stderr, "getrlimit failed! line = %d \n", __LINE__);
        exit(EXIT_FAILURE);
    }
    printf("After: cur is %ld, max is %ld \n", limit.rlim_cur, limit.rlim_max);

     if (mlock(p + 1, pageSize) == -1) {
        printf("mlock failed!\n");
    } else {
        printf("mlock success! \n");
    }

    munlock(p + 1, 2 * pageSize);
    
    munmap(p, 2 * pageSize);
    exit(EXIT_SUCCESS);
}