#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <myDef.h>
#include <syslog.h>
#include "id_echo.h"
#include "inet_sockets.h"

#define ERROUT(...) errPrintf(NULL, __LINE__, strerror(errno), __VA_ARGS__)

int main(char argc, char *argv[])
{
    int lfd;
    socklen_t addrlen;
    struct sockaddr sockaddr;
    socklen_t len;
    int numRead;
    char buf[BUF_SIZE];
    char addrbuf[100];

    lfd = inetBind(SERVER, SOCK_DGRAM, &addrlen);
    if (lfd == -1) {
        syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
        ERROUT("inetBind failed!");
    }

    while (1) {
        len = sizeof(sockaddr);
        numRead = recvfrom(lfd, (void *)buf, sizeof(buf), 0, &sockaddr, &len);
        if (numRead == -1) {
            ERROUT("recvfrom failed!");
        }

        if (sendto(lfd, (void *)buf, numRead, 0, &sockaddr, addrlen) != numRead) {
            syslog(LOG_ERR, "Error echoing respone to %s (%s)",
                inetAddressStr(&sockaddr, addrlen, addrbuf, sizeof(addrbuf)),
                               strerror(errno));
        }
    }

    exit(EXIT_SUCCESS);
}