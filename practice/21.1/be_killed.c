#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static void myabort(void)
{
    raise(SIGABRT);
}

int main(char argc, char *argv[])
{
    int n;

    while (1) {
        printf("print counts = %d PID = %d\n", n, getpid());
        n++;
        sleep(1);
        if (n == 3) {
            //abort();
            myabort();
        }
    }

    exit(EXIT_SUCCESS);
}