#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <myDef.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include "common.h"

#define DEBUG

#ifdef DEBUG
#define ERROUT(...) errPrintf(__FILE__, __LINE__, strerror(errno), ##__VA_ARGS__)
#else 
#define ERROUT(...)
#endif

static void recvRemoteA(char *path)
{
    int sk;
    struct sockaddr_un local, remote;
    int lenRead = 0, numRead = 0;
    char buf[1024];
    socklen_t skLen = 0;

    sk = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sk == -1) {
        ERROUT("create socket failed!");
    }

    memset((void *)&local, 0, sizeof(local));
    local.sun_family = AF_UNIX;
    snprintf(local.sun_path, sizeof(local.sun_path) - 1, "%s", path);

    if ((remove(path) == -1) && (errno != ENOENT)) {
        ERROUT("%s has existed and remove failed!", path);
    }

    if (bind(sk, (struct sockaddr *)&local, sizeof(local)) == -1) {
        ERROUT("bind failed!");
    }

    memset((void *)&remote, 0, sizeof(remote));

    while (1) {
        lenRead = recvfrom(sk, (void *)buf, sizeof(buf), 0,
                        (struct sockaddr *)&remote, &skLen);
        if (lenRead > 0) {
            printf("Serve recvfrom %s:\ndetail: %s \n", remote.sun_path, buf);
            memset((void *)buf, 0, sizeof(buf));
            memset((void *)&remote, 0, sizeof(remote));
        }
    }
}

int main(char argc, char *argv[])
{
   

    switch (fork()) {
        case -1:
            ERROUT("fork failed!");
            break;
        
        case 0: {
            printf("server Child PID is %ld\n", (long)getpid());
            recvRemoteA(SUN_PATHC);
            break;
        }

        default: {
            printf("server Parent PID is %ld\n", (long)getpid());
            recvRemoteA(SUN_PATHB);

            break;
        }    
    }

    waitpid(-1, NULL, 0);

    exit(EXIT_SUCCESS);
}