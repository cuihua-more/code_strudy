#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

static void errPrintf(int line, const char *fmt, ...)
{
    va_list ap;
    char buf[4096];

    memset(buf, 0, sizeof(buf));
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "line = %d\n",
             line);
    write(STDERR_FILENO, (void *)buf, strlen(buf));

    exit(EXIT_FAILURE);
}

int main(char argc, char *argv[])
{
    int toChild[2];
    int toParent[2];

    if (pipe(toChild) == -1) {
        errPrintf(__LINE__, "pipe  Child failed! line");
    }

    if (pipe(toParent) == -1) {
        errPrintf(__LINE__, "pipe Parent failed! line");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
        case -1:
            errPrintf(__LINE__, "fork failed! line");
            break;

        case 0:
            /* chile process */

            /*
             * child use STDIN_FILENO for read Parent process
             */
            close(toChild[1]);

            close(STDIN_FILENO);
            dup2(toChild[0], STDIN_FILENO);
            close(toChild[0]);

            /*
             * child use STDOUT_FILENO for write to Parent process
             */
            close(toParent[0]);

            close(STDOUT_FILENO);
            if (dup(toParent[1]) != STDOUT_FILENO) {
                dup2(toParent[1], STDOUT_FILENO);
            }
            close(toParent[1]);

            char chr, CHR;
            while (1) {
                if (read(STDIN_FILENO, (void *)&chr, sizeof(chr)) != sizeof(chr)) {
                    continue;
                }

                CHR = chr - 32;
                if (write(STDOUT_FILENO, (void *)&CHR, sizeof(CHR)) != sizeof(CHR)) {
                    continue;
                }
                if (chr == 'e') { 
                    close(STDOUT_FILENO);
                    close(STDIN_FILENO);
                    fprintf(stderr, "Child process Stop PID = %ld\n", (long)getpid());
                    exit(EXIT_SUCCESS);
                }
            }
            break;
            
        default :
            break;
    }

    /*
     * parent use STDOUT_FILENO for write Child process
     */
    close(toChild[0]);
    close(STDOUT_FILENO);
    dup2(toChild[1], STDOUT_FILENO);
    close(toChild[1]);

    /*
     * parent use STDIN_FILENO for read Child process
     */
    close(toParent[1]);
    close(STDIN_FILENO);
    dup2(toParent[0], STDIN_FILENO);
    close(toParent[0]);

    char chr = 'a';
    while (1) {
        if (write(STDOUT_FILENO, (void *)&chr, sizeof(chr)) != sizeof(chr)) {
            continue;
        }
        sleep(1);
        char CHR;
        if (read(STDIN_FILENO, (void *)&CHR, sizeof(CHR)) != sizeof(CHR)) {
            continue;
        }
        fprintf(stderr, "get child is %c\n", CHR);
        chr++;
        if ((chr - 'a') == 5) {
            close(STDOUT_FILENO);
            close(STDIN_FILENO);
            fprintf(stderr, "Parent process Stop PID = %ld\n", (long)getpid());
            break;
        }
    }

    exit(EXIT_SUCCESS);
}