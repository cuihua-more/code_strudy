#ifndef __TALK_COMMON_H_
#define __TALK_COMMON_H_

#define SERVER_ID_FILE_PATH     "/tmp/svmsg_talk_server.txt"
#define CLIENT_NUM_MAX          1024

#define CLIENT_NAME_LEN     48
#define MESSAGE_LEN         4096

struct taskMsg {
    long mtype;
    char targetName[CLIENT_NAME_LEN];
    char msg[MESSAGE_LEN];
};
#define SERVER_MSG_LEN (sizeof(struct taskMsg) - sizeof(long))

struct registerMsg {
    long mtype;
    int clientId;
    char clientName[CLIENT_NAME_LEN];
};
#define SERVER_REG_LEN  (sizeof(struct registerMsg) - sizeof(long))

#define MTYPE_REGISTER      1
#define MTYPE_TALK          2
#define MTYPE_DELETE        3

#endif  /* __TALK_COMMON_H_ */
