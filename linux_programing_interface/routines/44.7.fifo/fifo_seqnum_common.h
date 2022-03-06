#ifndef __FIFO_SEQNUM_COMMON_H_
#define __FIFO_SEQNUM_COMMON_H_
#include <sys/types.h>
#include <unistd.h>

#define SERVER_FIFO     "/tmp/seqnum_server"
#define CLIENT_FIFO_TEMPLATE    "/tmp/seqnum_client.%ld"
#define CLIENT_FIFO_NAME_LEN    (sizeof(CLIENT_FIFO_TEMPLATE) + 20)

struct request {
    pid_t pid;
    int seqLen;
};

struct response {
    int seqLen;
};

#endif  /* __FIFO_SEQNUM_COMMON_H_ */
