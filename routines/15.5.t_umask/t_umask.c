#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "file_perms.h"

#define MYFILE  "myfile"
#define MYDIR   "mydir"
#define FILE_PERMS      (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define DIR_PERMS       (S_IRWXU | S_IRWXG | S_IRWXO)
#define UMASK_SETTING   (S_IWUSR | S_IXUSR | S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH)

int main(char argc, char *argv[])
{
    int fd;
    struct stat sb;
    mode_t perms;

    umask(UMASK_SETTING);

    fd = open(MYFILE, O_CREAT | O_RDWR | O_EXCL, FILE_PERMS);
    if (fd == -1) {
        fprintf(stderr, "open failed! line: %d \n", __LINE__);
        exit(EXIT_FAILURE);
    }
    if (mkdir(MYDIR, DIR_PERMS) == -1) {
        fprintf(stderr, "mkdir failed! line: %d \n", __LINE__);
        exit(EXIT_FAILURE);
    }

    perms = umask(0);

    if (stat(MYFILE, &sb) ==-1) {
        fprintf(stderr, "stat failed! line: %d \n", __LINE__);
        exit(EXIT_FAILURE);
    }
    printf("Requested file perms:   %s\n", filePermStr(FILE_PERMS, 0));
    printf("Process Mask:           %s\n", filePermStr(perms, 0));
    printf("Actual perms:           %s\n\n", filePermStr(sb.st_mode, 0));

    if (stat(MYDIR, &sb) == -1) {
        fprintf(stderr, "stat failed! line: %d \n", __LINE__);
        exit(EXIT_FAILURE);
    }
    printf("Requested dir. perms:   %s\n", filePermStr(DIR_PERMS, 0));
    printf("Process Mask:           %s\n", filePermStr(perms, 0));
    printf("Actual perms:           %s\n\n", filePermStr(sb.st_mode, 0));

    if (unlink(MYFILE) == -1) {
        fprintf(stderr, "unlink %s failed! line: %d \n", MYFILE, __LINE__);
        exit(EXIT_FAILURE);
    }

    if (rmdir(MYDIR) == -1) {
        fprintf(stderr, "rmdir %s failed! line: %d \n", MYDIR, __LINE__);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}