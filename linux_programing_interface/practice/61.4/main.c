#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <myDef.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
 #include <arpa/inet.h>

#define ERROUT(...) errPrintf(NULL, __LINE__, strerror(errno), __VA_ARGS__)

int main(char argc, char *argv[])
{
    int lfd;
    struct sockaddr addr;
    socklen_t addrLen;
    char addrbuf[1024];

    lfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (lfd == -1) {
        ERROUT("socket failed!");
    }

    if (listen(lfd, 10) == -1) {
        ERROUT("listen failed!");
    }

    if (getsockname(lfd, &addr, &addrLen) == -1) {
        ERROUT("getsockname failed!");
    }

    printf("addrLen is %d\n", addrLen);

    if (addrLen == sizeof(struct sockaddr_in)) {
        printf("Cur is IPv4, Addr is %s Port is %d\n", 
        inet_ntop(AF_INET, (void *)&((struct sockaddr_in *)&addr)->sin_addr, addrbuf, sizeof(addrbuf)),
            ((struct sockaddr_in *)&addr)->sin_port);
    } else {
        printf("Cur is IPv6, Addr is %s Port is %d\n",
        inet_ntop(AF_INET, (void *)&((struct sockaddr_in6 *)&addr)->sin6_addr, addrbuf, sizeof(addrbuf)),
            ((struct sockaddr_in6 *)&addr)->sin6_port);
    }

    exit(EXIT_SUCCESS);
}