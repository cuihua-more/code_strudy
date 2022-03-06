#ifndef _SVSHM_XFR_H_
#define _SVSHM_SFR_H_
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SEM_KEY     0x123456
#define SHM_KEY     0x456789

#define XFR_PERMISSION  S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP

/*
 * sem op num
 */
#define MUTEX_WRITE 1
#define MUTEX_READ  0

union semun {
    int val;                    /* use for SETVAl or GETVAL */
    struct semid_ds *buf;       /* use for IPC_STAT or IPC_SET */
    unsigned short *arrray;     /* use for SETALL ot GETALL */
    struct seminfo *__buf;      /* use for IPC_INFO  */
};

#ifndef BUF_SIZE    
#define BUF_SIZE    1024
#endif

struct shm {
    int length;
    char msg[BUF_SIZE];
};

#endif  /* _SVSHM_SFR_H_ */
