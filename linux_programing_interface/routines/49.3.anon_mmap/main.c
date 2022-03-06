
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define USE_MAP_ANON

int main(char argc, char *argv[])
{
    int *p = NULL;

#ifdef USE_MAP_ANON
    p = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED,
             -1, 0);
    if (p == NULL) {
        fprintf(stderr, "mmap failed! line = %d %s \n", __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }
#else
    int fd;
    fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "open /dev/zero device failed! line = %d %s \n",
                __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }
    p = mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (p == NULL) {
        fprintf(stderr, "mmap failed! line = %d %s \n", __LINE__, strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
#endif

    *p = 1;

    switch(fork()) {
        case -1 :
            fprintf(stderr, "fork failed! \n");
            exit(EXIT_FAILURE);
            break;

        case 0:
            /* child process */
            printf("Child start. Value = %d\n", *p);
            (*p)++;

            if (munmap(p, sizeof(int)) == -1) {
                fprintf(stderr, "Child: munmap failed! \n");
                exit(EXIT_FAILURE);
            }

            exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }

    if (wait(NULL) == -1) {
        fprintf(stderr, "Parent: wait child process failed! %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("In parent. Value = %d\n", *p);
    if (munmap(p, sizeof(int)) == -1) {
        fprintf(stderr, "Parent: munmap failed! \n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
