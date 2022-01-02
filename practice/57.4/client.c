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
#include "common.h"

#define DEBUG

#ifdef DEBUG
#define ERROUT(...) errPrintf(__FILE__, __LINE__, strerror(errno), ##__VA_ARGS__)
#else 
#define ERROUT(...)
#endif

int main(char argc, char *argv[])
{
    int sk;
    struct sockaddr_un local, remoteB, remoteC;
    socklen_t lenSk;
    int lenSend;

    sk = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sk == -1) {
        ERROUT("create socket failed!");
    }

    memset((void *)&local, 0, sizeof(local));
    local.sun_family = AF_UNIX;
    /*
     * 地址从第二个字节开始
     * 表示撞见的是抽象sock空间
     */
    snprintf(&local.sun_path[1], sizeof(local.sun_path) - 2, "%s", SUN_PATHA);
    if (bind(sk, (struct sockaddr *)&local, sizeof(local)) == -1) {
        ERROUT("bind failed!");
    }

    memset((void *)&remoteB, 0, sizeof(remoteB));
    remoteB.sun_family = AF_UNIX;
    snprintf(remoteB.sun_path, sizeof(remoteB.sun_path) - 1, "%s", SUN_PATHB);
    if (connect(sk, (struct sockaddr *)&remoteB, sizeof(remoteB)) == -1) {
        ERROUT("connect failed!");
    }

    memset((void *)&remoteC, 0, sizeof(remoteC));
    remoteC.sun_family = AF_UNIX;
    snprintf(remoteC.sun_path, sizeof(remoteC.sun_path) - 1, "%s", SUN_PATHC);

    printf("send to %s:\n", SUN_PATHB);
    lenSend = send(sk, (argc > 1) ? argv[1] : "hello word!", (argc > 1) ? strlen(argv[1]) : 10, 0);
    if (lenSend  == -1) {
        ERROUT("send failed!");
    }

    sleep(1);

    printf("sendto to %s:\n", SUN_PATHC);
    lenSend = sendto(sk, (argc > 1) ? argv[1] : "hello word!", (argc > 1) ? strlen(argv[1]) : 10, 0,
                     (struct sockaddr *)&remoteC, sizeof(remoteC));
    if (lenSend == -1) {
        ERROUT("sendto failed!");
    }

    exit(EXIT_SUCCESS);
}