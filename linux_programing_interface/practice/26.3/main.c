#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "fcntl.h"
#include "print_wait_status.h"
#include <sys/wait.h>

int main(char argc, char *argv[])
{
    pid_t child;
    int status;

    if ((argc > 1) && (strcmp(argv[1], "--help") == 0)) {
        printf("Usage: %s [exit-status]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
        case -1:
            fprintf(stderr, "fork error! line: %d\n", __LINE__);
            exit(EXIT_FAILURE);
        case 0:
            printf("Child started with PID = %ld \n", (long)getpid());

            if (argc > 1) {
                exit((int)atol(argv[1]));
            } else {
                while (1) {
                    pause();
                }
                exit(EXIT_FAILURE);
            }
        default:
            while (1) {
                child = waitpid(-1, &status, WUNTRACED | WCONTINUED);

                if (child == -1) {
                    fprintf(stderr, "waitpid failed! line:%d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }

                printf("waitpid() returned: PID=%ld; status=0x%04x (%d, %d)\n",
                                                                    (long)child,
                                                                    (unsigned int)status,
                                                                    status >> 8,
                                                                    status & 0xff);
                printWaitStatus(NULL, status);
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    exit(EXIT_SUCCESS);
                }
            }
            
        }

    exit(EXIT_SUCCESS);
}