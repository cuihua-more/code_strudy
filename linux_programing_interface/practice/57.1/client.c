#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <myDef.h>
#include "common.h"

#define debug

#ifdef debug
#define ERROUT(...) errPrintf(__FILE__, __LINE__, strerror(errno), ##__VA_ARGS__)
#else 
#define ERROUT(...)
#endif

int main(char argc, char *argv[])
{
    int std;
    struct sockaddr_un addr, server;
    int len, lens;
    char buf[10];
    int sendnum = 0;

    std = socket(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (std == -1) {
        ERROUT("create socket failed!");
    }

    memset((void *)&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path) - 1, "%s.%ld", SUM_PATH, (long)getpid());
    if (bind(std, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        ERROUT("bind failed!");
    }

    memset((void *)&server, 0, sizeof(server));
    server.sun_family = AF_UNIX;
    snprintf(server.sun_path, sizeof(server.sun_path) - 1, "%s", SUM_PATH);

    while (1) {
        len = read(STDIN_FILENO, (void *)buf, sizeof(buf));
        if (len <= 0) {
            break;
        }

        lens = sendto(std, buf, len, 0, (struct sockaddr *)&server, sizeof(server));
        if (lens < len ) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                printf("client sendto block return\n");
                while (sendto(std, buf, len, 0, (struct sockaddr *)&server, sizeof(server)) != len);
            }
        }
        sendnum += lens;
        printf("%s has sended %d bytes\n", argv[0], sendnum);
    }

    printf("client end!\n");
    exit(EXIT_SUCCESS);
}