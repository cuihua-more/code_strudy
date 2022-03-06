#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>

static void errExit(int line, const char *format,...)
{
    va_list ap;
    char buf[1024];

    va_start(ap, format);

    vsprintf(buf, format, ap);

    va_end(ap);

    fprintf(stderr, "%s line:%d\n", buf, line);

    exit(EXIT_FAILURE);
}

int main(char argc, char *argv[])
{
    int sigNum, sig, n;
    pid_t pid;

    if (argc < 4 || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s pid num-sigs sig-num [sig-num-2]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = atol(argv[1]);
    sigNum = atol(argv[2]);
    sig = atol(argv[3]);

    for (n = 0; n < sigNum; n++) {
        if (kill(pid, sig) == -1) {
            errExit(__LINE__, "kill");
        }
    }

    if (argc > 4) {
        if (kill(pid, atol(argv[4])) == -1 ) {
            errExit(__LINE__, "kill");
        }
    }

    printf("%s: exiting \n", argv[0]);

    exit(EXIT_SUCCESS);
}