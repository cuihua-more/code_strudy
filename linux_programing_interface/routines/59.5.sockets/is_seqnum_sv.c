#define _GNU_SOURCE
#include "is_seqnum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <myDef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

#define ERROUT(...) errPrintf(NULL, __LINE__, strerror(errno), __VA_ARGS__)

int main(char argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *paddr, *p;
    struct sockaddr_in *ipv4;
    char strIpv4[INET_ADDRSTRLEN];

    memset((void *)&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = 0;
    hints.ai_socktype = 0; //TCP
    //hints.ai_flags =  AI_NUMERICSERV | AI_PASSIVE ;

    if (getaddrinfo("www.baidu.com", NULL, &hints, &paddr)) {
        ERROUT("getaddrinfo failed!");
    }

    printf("ai_canonname is %s \n", paddr->ai_canonname);

    for (p = paddr; p != NULL; p = paddr->ai_next) {
        ipv4 = (struct sockaddr_in *)p->ai_addr;
        if (inet_ntop(p->ai_family, (void *)ipv4, strIpv4, sizeof(strIpv4)) == NULL) {
            ERROUT("inet_ntop failed!");
        }
        printf("Addr is %s  Port is %d \n", strIpv4, ipv4->sin_port);
    }

    freeaddrinfo(paddr);

    exit(EXIT_SUCCESS);
}