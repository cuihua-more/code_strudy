#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>      
#include <sys/stat.h>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>

static uid_t getIdFormNmae(const char *name)
{
    struct passwd *pw;

    if (name == NULL) {
        return -1;
    }

    pw = getpwnam(name);

    return (pw == NULL) ? -1 : pw->pw_uid;
}

int main(char argc, char *argv[])
{
    uid_t uid;
    int fd;
    DIR *dir;
    struct dirent *dirent;
    char path[1024];
    char file[1024];
    ssize_t numRd;
    char *temp1, *temp2;
    int id;
    char *pid;
    char *name;

    if ((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s UserName\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    uid = getIdFormNmae(argv[1]);
    if (uid == -1) {
        fprintf(stderr, "getIdFromName failed! line: %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("%s's uid = %d\n", argv[1], uid);

    dir = opendir("/proc");
    if (dir == NULL) {
        fprintf(stderr, "open /proc dir failed! line: %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    dirent = readdir(dir);
    while (dirent) {
        //printf("/proc/%s\n", dirent->d_name);
        sprintf(path, "/proc/%s/status", dirent->d_name);
        fd = open(path, O_RDONLY);
        if (fd != -1) {
            numRd =  read(fd, file, 1024);
            if (numRd == 0) {
                close(fd);
                continue;
            }

            temp1 = strstr(file, "Uid:");
            while(*temp1++ != 0x9);
            temp2 = strchr(temp1, 0x9);
            *temp2 = 0;
            id = atol(temp1);
            if (id == uid) {
                temp1 = strstr(file, "Pid:");
                if (temp1 != NULL) {
                    while(*temp1++ != 0x9);
                    temp2 = strstr(temp1, "\n");
                    *temp2 = 0;
                    pid = temp1;
                    temp1 = strstr(file, "\n");
                    *temp1 = 0;
                    name = file;
                    printf("UserName=%s's pthread %s    pid = %s\n", argv[1], name,pid);
                }
            }

            memset(file, 0, 1024);
            close(fd);
            memset(path, 0, strlen(path));
        }
        dirent = readdir(dir);
    }

    closedir(dir);

    exit(EXIT_SUCCESS);
}