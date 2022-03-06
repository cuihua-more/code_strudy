#define _SNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>

static void errPrintf(int line, const char *fmt, ...)
{
    va_list ap;
    char buf[4096];

    memset((void *)buf, 0, sizeof(buf));
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " line is %d ", line);
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " %s \n", strerror(errno));
    write(STDERR_FILENO, (void *)buf, strlen(buf));
    exit(EXIT_FAILURE);
}
#define ERROUT(...) errPrintf(__LINE__, __VA_ARGS__)

static void bus_Handler(int signa)
{
    printf("Receive SIGBUS(%d) signal \n", signa);
    signal(SIGBUS, SIG_IGN);
}

static void segv_Handler(int signal)
{
    printf("Receive SIGSEGV(%d) signal \n", signal);
}

int main(char argc, char argv[])
{
    char *p = NULL;
    int fd;
    int i;
    int len = sysconf(_SC_PAGE_SIZE);

    fd = open("temp", O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);
    if (fd == -1) {
        ERROUT("open temp failed!");
    }

    write(fd, "1234", 4);
    close(fd);

    fd = open("temp", O_RDWR);
    if (fd == -1) {
        ERROUT("open temp failed!");
    }

    p = mmap(NULL, len * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        ERROUT("mmap failed!");
    }

    signal(SIGBUS, bus_Handler);
    signal(SIGSEGV, segv_Handler);

    printf("%c\n", p[0]);
    printf("%c\n", p[5]);
    printf("%c\n", p[len]);             /* SIGBUS */
    printf("%c\n", p[2 * len]);         /* SIGSEGV */


    exit(EXIT_SUCCESS);
}