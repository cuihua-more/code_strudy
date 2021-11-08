#ifndef _SVMSG_FILE_H_
#define _SVMSG_FILE_H_

#define SERVER_KEY  0xA55A1234

#define SERVER_KEY_FILE     "/tmp/svmsg_id.txt"

#define PATH_MAX    512

struct requestMsg {
    long mtype;
    int chiletId;
    char pathName[PATH_MAX];
};

#define REQ_MSG_SIZE    (sizeof(struct requestMsg) - sizeof(long))

#define RESP_MSG_SIZE   8192

struct responseMsg {
    long mtype;
    char data[RESP_MSG_SIZE];
};

/* Types for response messages sent from server to client */
#define RESP_MT_FAILURE 1
#define RESP_MT_DATA    2
#define RESP_MT_END     3

#endif /* _SVMSG_FILE_H_ */