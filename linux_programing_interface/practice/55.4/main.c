#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <myDef.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/file.h>

#define TEMP1_PATH  "temp2.txt"
#define DEBUG

#ifdef DEBUG
    #define ERROUT(...)  errPrintf(NULL, __LINE__, strerror(errno), ##__VA_ARGS__)   
#else
    #define ERROUT(...) 
#endif

static void usage(char *str)
{
    fprintf(stderr, "Usage: %s [c/l][R/W[num]] [c/l][R/W[num]]\n", str);
    fprintf(stderr, "           c:使用fcntl加锁\n");
    fprintf(stderr, "           l:使用flock加锁\n");
    fprintf(stderr, "               R:放置一把读锁\n");
    fprintf(stderr, "               W:放置一把写锁\n");
    fprintf(stderr, "                   num:(可选)加锁的长度，单位字节，仅从当前位置开始，默认长度是当前位置到文件末尾，仅在fcntl方式加锁有效\n");
    fprintf(stderr, "Examlpe: %s cW20 lW\n", str);
    fprintf(stderr, "表示对一个文件先使用fcnlt在文件当前位置到20个字节后，加一把写锁，然后使用flock加一把写锁，判断flock是否执行成功\n");
    exit(EXIT_FAILURE);
}

static void resultOut(char *argv[])
{
    printf("在第一次使用 %s 加 %s 锁后，本次 %s 加 %s 锁失败\n", (argv[1][0] == 'c') ? "fcntl" : 
                                                              (argv[1][0] == 'l') ? "flock" : "error",
                                                              (argv[1][1] == 'R') ? "读"    :
                                                              (argv[1][1] == 'W') ? "写"    : "error",
                                                              (argv[2][0] == 'c') ? "fcntl" : 
                                                              (argv[2][0] == 'l') ? "flock" : "error",
                                                              (argv[2][1] == 'R') ? "读"    :
                                                              (argv[2][1] == 'W') ? "写"    : "error");
    printf("失败原因: %s \n", strerror(errno));
}

int main(char argc, char *argv[])
{
    int fd1;
    struct flock lock;
    char i = 0;

    if ((argc != 3) || (strcmp(argv[1], "--help") == 0)) {
        usage(argv[0]);
    }

    fd1 = open(TEMP1_PATH, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);
    if (fd1 == -1) {
        ERROUT("create %s file failed!", TEMP1_PATH);
    }

    close(fd1);

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == 'c') {
            if (argv[i][1] == 'R') {
                lock.l_type = F_RDLCK;
            } else if (argv[i][1] == 'W') {
                lock.l_type = F_WRLCK;
            } else {
                fprintf(stderr, "%s 的第二个字符 %c 错误!\n", argv[i], argv[i][1]);
            }

            if (argv[i][2] != '\0') {
                lock.l_len = (off_t)atol(&argv[i][2]);
            } else {
                lock.l_len = 0;
            }

            lock.l_start = 0;
            lock.l_whence = SEEK_CUR;

            if (fork() == 0) {
                printf("当时是第 %d 次, PID is %ld\n", i, (long)getpid());
                system("ls");
                int fd;

                fd = open(TEMP1_PATH, O_RDWR);
                if (fd == -1) {
                    ERROUT("open %s file failed!\n", TEMP1_PATH);
                } else {
                    printf("第 %d 次打开 %s 文件成功\n", i, TEMP1_PATH);
                }

                 if (fcntl(fd, F_SETLK, &lock) == -1) {
                    if (((errno == EAGAIN) || (errno == EACCES)) && (i == 2)) {
                        resultOut(argv);
                        exit(EXIT_SUCCESS);
                    } else {
                        ERROUT("第%d次失败\n", i);
                    }
                } else {
                    printf("第%d次加锁成功, sleep 1 s\n", i);
                    sleep(1);
                }
            }
           
        } else if (argv[i][0] == 'l') {
            int cmd;
            if (argv[i][1] == 'R') {
                cmd = LOCK_SH;
            } else if (argv[i][1] == 'W') {
                cmd = LOCK_EX;
            } else {
                fprintf(stderr, "%s 的第二个字符 %c 错误!\n", argv[i], argv[i][1]);
            }

            if (fork() == 0) {
                printf("当时是第 %d 次, PID is %ld\n", i, (long)getpid());
                system("ls");
                int fd;

                fd = open(TEMP1_PATH, O_RDWR);
                if (fd == -1) {
                    ERROUT("open %s file failed!\n", TEMP1_PATH);
                } else {
                    printf("第 %d 次打开 %s 文件成功\n", i, TEMP1_PATH);
                }

                if (flock(fd, cmd | LOCK_NB) == -1) {
                    if ((errno == EWOULDBLOCK) && (i == 2)) {
                        resultOut(argv);
                        exit(EXIT_SUCCESS);
                    } else {
                        ERROUT("第%d次失败\n", i);
                    }
                } else {
                    printf("第%d次加锁成功 sleep 1 s\n", i);
                    sleep(1);
                }
            } 
        } else {
            fprintf(stderr, "%s 的第一个字符 %c 错误!\n", argv[i], argv[i][0]);
            usage(argv[0]);
        }
    }

    waitpid(-1, NULL, 0);
    waitpid(-1, NULL, 0);
    //remove(TEMP1_PATH);
    exit(EXIT_SUCCESS);
}