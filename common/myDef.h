#ifndef _MY_DEF_H_
#define _MY_DEF_H_

void errPrintf(char *file, int line, char *err, char *fmt, ...);

#include <sys/socket.h>

/**
 *  @func: 根据输入的地址和端口号，创建客户端，并进行连接
 *  @param - host   : 要连接的地址，可以是字符形式的地址如 "192.168.1.2"
 *  @param - server : 要连接的端口号，可以是字符形式的数字例如 "5000" 或者 某个众所周知的服务名称例如 "ftp"
 *  @param - type   : 要绑定的类型，TCP：SOCK_STREAM UCP：SOCK_DGRAM
 *  @returm: -1 失败，其他 socket
*/
int inetConnect(const char *host, const char *service, int type);

/**
 *  @func: 根据端口号创建一个已经处于监听状态的服务器socket
 *  @param - server : 要绑定的端口号，可以是字符形式的数字例如 "5000" 或者 某个众所周知的服务名称例如 "ftp"
 *  @param - backlog: 处于连接状态时可以最大接受连接的数量
 *  @param - addrlen: 绑定后的sockaddr的长度，可以根据此判断是IPv4或者IPv6
 *  @returm: -1 失败，其他 socket
*/
int inetListen(const char *server, int backlog, socklen_t *addrlen);

/**
 *  @func: 通过传递的端口号快速绑定到本地地址
 *  @param - server : 要绑定的端口号，可以是字符形式的数字例如 "5000" 或者 某个众所周知的服务名称例如 "ftp"
 *  @param - type   : 要绑定的类型，TCP：SOCK_STREAM UCP：SOCK_DGRAM
 *  @param - addrlen: 绑定后的sockaddr的长度，可以根据此判断是IPv4或者IPv6
 *  @returm: -1 失败，其他 socket
*/
int inetBind(const char *server, int type, socklen_t *addrlen);

/**
 *  @func: 获取字符形式的IP地址和端口
 *  @param - addr   : 地址结构体
 *  @param - addrlem: 地址结构体长度
 *  @param - addrStr: 存放字符的传冲区首地址
 *  @param - addrStrLen: 缓冲区长度
 *  @return: 缓冲区首地址
*/
char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen);

#endif  /* _MY_DEF_H_ */