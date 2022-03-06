#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>

static int sigCnt[NSIG];

static volatile sig_atomic_t gotSignal = 0;

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

static void printSigset(FILE *fd, const char *buf, sigset_t *sigset)
{
    int sig, i;

    for (sig = 1; sig < NSIG; sig++) {
        if (sigismember(sigset, sig)) {
            i++;
            fprintf(fd, "%s%d (%s)\n", buf, sig, strsignal(sig));
        }
    }

    if (i == 0) {
        fprintf(fd, "%s <empty signal set>\n", buf);
    }
}

static void sig_handler(int sig)
{
    if (sig == SIGINT) {
        gotSignal = 1;
    } else {
        sigCnt[sig]++;
    }
}

int main(char argc, char *argv[])
{
    int n;
    struct sigaction sigac1;
    sigset_t sigempty, sigMask, sigpend;
    int numSec;

    printf("%s: PID is %ld\n", argv[0],(long)getpid());

    sigac1.sa_handler = sig_handler;
    sigac1.sa_flags = 0;
    sigemptyset(&sigac1.sa_mask);

    for (n = 1; n < NSIG - 1; n++) {
        sigaction(n, &sigac1, NULL);
    }

    if (argc > 1) {
        numSec = (int)atol(argv[1]);

        sigfillset(&sigMask);
        if (sigprocmask(SIG_SETMASK, &sigMask, NULL) == -1) {
            errExit(__LINE__, "sigprocmask");
        }

        printf("%s sleeping for %d seconds\n", argv[0], numSec);
        sleep(numSec);

        if (sigpending(&sigpend) == -1) {
            errExit(__LINE__, "sigpending");
        }

        printf("%s recvive signal is \n", argv[0]);
        printSigset(stderr, "\t\t", &sigpend);

        sigemptyset(&sigMask);
        if (sigprocmask(SIG_SETMASK, &sigMask, NULL) == -1) {
            errExit(__LINE__, "sigprocmask");
        }
    }

    while (!gotSignal) {
        continue;
    }

    for (n = 0; n < NSIG; n++) {
        if (sigCnt[n] != 0) {
            printf("%s: signal %d caught %d time%s\n", argv[0], n, sigCnt[n], (sigCnt[n] == 1) ? "" : "s");
        }
    }

        exit(EXIT_SUCCESS);
}