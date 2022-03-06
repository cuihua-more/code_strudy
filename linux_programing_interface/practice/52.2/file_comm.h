#ifndef __FILE_COMM_H_
#define __FILE_COMM_H_
#include <stdarg.h>

#define SERVER_MQ       "/file_server"

struct mq_request {
    char client_mq_name[128];
    char file_name[256];
};

struct mq_response {
    int flags;
    char data[4096];
    int dataLen;
};

#define RESP_FAILE      1
#define RESP_DATA       2
#define RESP_END        3

void errPrintf(char *file, int line, char *fmt, ...);
#define ERROUT(...) errPrintf(__FILE__, __LINE__, __VA_ARGS__)

#endif /* __FILE_COMM_H_ */