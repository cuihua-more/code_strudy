#define _GNU_SOURCE
#include <stdio.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <stdlib.h>

int main(char argc, char *argv[])
{

    struct utsname uts;

    if (uname(&uts) == -1) {
        fprintf(stderr, "call uname() cailed! line:%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("Node name:      %s\n", uts.nodename);
    printf("System name:    %s\n", uts.sysname);
    printf("Release:        %s\n", uts.release);
    printf("Version:        %s\n", uts.version);
    printf("Machine:        %s\n", uts.machine);

#ifdef _GNU_SOURCE
    printf("Domain name:    %s\n", uts.domainname);
#endif // _GNU_SOURCE
    exit(EXIT_SUCCESS);
}