#define _GNU_SOURCE
#include "fifo_seqnum_common.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

int main(char argc, char *argv[])
{
    int serverFd, clientFifo;
    struct request req;
    struct response resp;
    int respNum = 0;
}