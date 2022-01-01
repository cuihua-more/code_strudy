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
    struct sockaddr_un addr, client;
    char flag = 0;
    char buf[10];
    int len;
    socklen_t socklen;

    std = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (std == -1) {
        ERROUT("create socket failed!");
    }

    memset((void *)&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path) - 1, SUM_PATH);

    if (bind(std, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        ERROUT("bind failed!");
    }

    while (1) {
        len = recvfrom(std, (void *)buf, sizeof(buf), 0, (struct sockaddr *)&client, &socklen);
        if (len < 0) {
            ERROUT("server recvfrom failed!");
        }
        if (flag == 0) {
            printf("server recv PATH is %s \n", client.sun_path);
        }
        flag++;
        write(STDOUT_FILENO, (buf), len);
        sleep(1);
    }
}