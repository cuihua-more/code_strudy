#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <myDef.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
extern int h_errno;

#define ERROUT(...) errPrintf(NULL, __LINE__, strerror(errno), __VA_ARGS__)

int main(char argc, char *argv[])
{
    struct hostent *h;
    char **pp;
    char buf[INET6_ADDRSTRLEN];

    for (argv++; *argv != NULL; argv++) {
        h = gethostbyname(*argv);
        if (h == NULL) {
            fprintf(stderr, "%s \n", "gethostbyname failed!");
            continue;
        }

        printf("Official name of host is %s \n", h->h_name);

        printf("        alias(es):      ");
        for (pp = h->h_aliases; (*pp) != NULL; pp++) {
            printf("%s ", *pp);
        }
        printf("\n");

        printf("        address type:   %s \n", (h->h_addrtype == AF_INET) ? "AF_INET"  : 
                                                (h->h_addrtype == AF_INET6) ? "AF_INT6" : "???");
    
        if ((h->h_addrtype == AF_INET6) || (h->h_addrtype == AF_INET)) {
            printf("        address(es):    ");
            for (pp == h->h_addr_list; (*pp != NULL); pp++) {
                if (!inet_ntop(h->h_addrtype, (void *)*pp, buf, INET6_ADDRSTRLEN)) {
                    fprintf(stderr, "inet_ntop error is %s\n", strerror(errno));
                }
                printf("%s ", buf);
            }
            printf("\n");
        }
    }
        exit(EXIT_SUCCESS);
}