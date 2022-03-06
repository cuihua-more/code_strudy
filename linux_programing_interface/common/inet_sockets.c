#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

/**
 *  @func: 根据输入的地址和端口号，创建客户端，并进行连接
 *  @param - host   : 要连接的地址，可以是字符形式的地址如 "192.168.1.2"
 *  @param - server : 要连接的端口号，可以是字符形式的数字例如 "5000" 或者 某个众所周知的服务名称例如 "ftp"
 *  @param - type   : 要绑定的类型，TCP：SOCK_STREAM UCP：SOCK_DGRAM
 *  @returm: -1 失败，其他 socket
*/
int inetConnect(const char *host, const char *service, int type)
{
    struct addrinfo hints;
    struct addrinfo *pAddr, *rp;
    int s;
    int sfd;

    memset((void *)&hints, 0, sizeof(hints));
    hints.ai_socktype = type;
    hints.ai_protocol = 0;
    hints.ai_family = AF_UNSPEC;    // 表示IPv4或者IPv6任意

    s = getaddrinfo(host, service, &hints, &pAddr);
    if (s != 0) {
        errno = ENOSYS;
        return -1;
    }

    for (rp = pAddr; rp != NULL; rp = pAddr->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            break;
        }

        close(sfd);
    }

    freeaddrinfo(rp);

    return (rp == NULL) ? -1 : sfd;
}

static int inetPassiveSocket(const char *server, int type, socklen_t *addrlen, char doListen, int backlog)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s;
    int lfd;
    int option;

    memset((void *)&hints, 0, sizeof(hints));
    hints.ai_socktype = type;       // TCP UDP from type
    hints.ai_family = AF_UNSPEC;    // IPv4 or IPv6
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, server, &hints, &result);
    if (s != 0) {
        return -1;
    }

    for (rp = result; rp != NULL; rp = result->ai_next) {
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1) {
            continue;
        }

        if (doListen) {
            /*
             * 表示创建一个服务器，需要设置lfd SO_REUSEADDR 表示端口复用
             */
            option = 1;
            if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, (void *)&option, sizeof(option)) == -1) {
                close(lfd);
                freeaddrinfo(result);
                return -1;
            }
        }

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }

        close(lfd);
    }

    if ((rp != NULL) && (doListen)) {
        if (listen(lfd, backlog) == -1) {
            freeaddrinfo(result);
            return -1;
        }
    }

    if ((rp != NULL) && (addrlen != NULL)) {
        *addrlen = rp->ai_addrlen;
    }

    freeaddrinfo(result);

    return (result == NULL) ? -1 : lfd;
}

/**
 *  @func: 根据端口号创建一个已经处于监听状态的服务器socket
 *  @param - server : 要绑定的端口号，可以是字符形式的数字例如 "5000" 或者 某个众所周知的服务名称例如 "ftp"
 *  @param - backlog: 处于连接状态时可以最大接受连接的数量
 *  @param - addrlen: 绑定后的sockaddr的长度，可以根据此判断是IPv4或者IPv6
 *  @returm: -1 失败，其他 socket
*/
int inetListen(const char *server, int backlog, socklen_t *addrlen)
{
    return inetPassiveSocket(server, SOCK_STREAM, addrlen, 1, backlog);
}

/**
 *  @func: 通过传递的端口号快速绑定到本地地址
 *  @param - server : 要绑定的端口号，可以是字符形式的数字例如 "5000" 或者 某个众所周知的服务名称例如 "ftp"
 *  @param - type   : 要绑定的类型，TCP：SOCK_STREAM UCP：SOCK_DGRAM
 *  @param - addrlen: 绑定后的sockaddr的长度，可以根据此判断是IPv4或者IPv6
 *  @returm: -1 失败，其他 socket
*/
int inetBind(const char *server, int type, socklen_t *addrlen)
{
    return inetPassiveSocket(server, type, addrlen, 0, 0);
}

/**
 *  @func: 获取字符形式的IP地址和端口
 *  @param - addr   : 地址结构体
 *  @param - addrlem: 地址结构体长度
 *  @param - addrStr: 存放字符的传冲区首地址
 *  @param - addrStrLen: 缓冲区长度
 *  @return: 缓冲区首地址
*/
char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen)
{
    char host[NI_MAXHOST];
    char server[NI_MAXSERV];

    if (getnameinfo(addr, addrlen, host, sizeof(host), server, 
                    sizeof(server), NI_NUMERICSERV) == -1) {
        snprintf(addrStr, addrStrLen, "(%s %s)", host, server);
    } else {
        snprintf(addrStr, addrStrLen, "(?UNKNOWN?)");
    }

    addrStr[addrStrLen - 1] = '\0';

    return addrStr;
}
