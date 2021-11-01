#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(char argc, char *argv[])
{
    int pipeFd[2];

    if (pipe(pipeFd)) {
        fprintf(stderr, "pipe fail!\n");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
        case -1:
            fprintf(stderr, "fork failed! line: %d\n", __LINE__);
            exit(EXIT_FAILURE);
        case 0:
            /* child  */
            close(pipeFd[1]);           /* close write */

            close(STDIN_FILENO);
            if (dup(pipeFd[0]) != STDIN_FILENO) {
                dup2(pipeFd[0], STDIN_FILENO);
                close(pipeFd[0]);
            }

            printf("child sleep 3s \n");
            sleep(3);

            char buf[10];
            while (1) {
                memset(buf, 0, sizeof(buf));
                if (read(STDIN_FILENO, (void *)buf, sizeof(buf)) == 0) {
                    break;
                } 
                printf("child PID = %ld : %s\n", (long)getpid(), buf);
            }
            exit(EXIT_SUCCESS);
        
        default:
            break;
    }

    close(pipeFd[0]);   /* close read */
    
    close(STDOUT_FILENO);
    if (dup(pipeFd[1]) != STDOUT_FILENO) {
        dup2(pipeFd[1], STDOUT_FILENO);
    }

    close(pipeFd[1]);

    printf("parent PID = %ld\n", (long)getpid());   /* invalid */

    char buf[10];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "line = %d", __LINE__);
    write(STDOUT_FILENO, (void *)buf, sizeof(buf));

    fprintf(stdout, "This fprintf");                /* invalid */

    close(STDOUT_FILENO);

    wait(NULL);

    exit(EXIT_SUCCESS);
}