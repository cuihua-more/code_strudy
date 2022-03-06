#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/sysmacros.h>
#include "file_perms.h"

static void printfStat(struct stat *sb)
{
    printf("File type:                  ");

    switch (sb->st_mode & S_IFMT) {
        case S_IFREG:
            printf("regular file\n"); break;
        case S_IFDIR:
            printf("directory\n"); break;
        case S_IFCHR:
            printf("character device\n"); break;
        case S_IFBLK:
            printf("black device\n"); break;
        case S_IFIFO:
            printf("fifo or pipe\n"); break;
        case S_IFSOCK:
            printf("sock\n");  break;
        case S_IFLNK:
            printf("symbolic (soft) link\n"); break;
        default:
            printf("Unknow file type!\n"); break;
    }

    printf("Device containing i-node:   major=%d minor=%d\n",
           major(sb->st_dev), minor(sb->st_dev));

    printf("I-node number:              %ld\n", (long)sb->st_ino);

    printf("Mode:                       %lo (%s)\n",
           (long)sb->st_mode, filePermStr(sb->st_mode, 0)); 
    if (sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX)) {
        printf("    special bits set:       %s%s%s\n",
               ((sb->st_mode & S_ISUID) ? "set-UID" : ""),
               ((sb->st_mode & S_ISGID) ? "set-GID" : ""),
               ((sb->st_mode & S_ISVTX) ? "sticky" : ""));
    }

    printf("Numbers of (hard) links:    %ld\n", (long)sb->st_nlink);

    printf("Ownership:                  UID=%ld  GID=%ld\n",
           (long)sb->st_uid, (long)sb->st_gid);

    if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode)) {
        printf("Device number: (st_rdev)    major=%ld    minor=%ld\n",
               (long)major(sb->st_rdev), (long)minor(sb->st_rdev));
    }

    printf("File size:                  %lld\n", (long long)sb->st_size);

    printf("Optimal I/O block size:     %ld\n", (long)sb->st_blksize);

    printf("512B blocks allocated:      %lld\n", (long long)sb->st_blocks);

    printf("Last file access:           Sec=%ld     Nanosec=%ld\n",
           sb->st_atim.tv_sec, sb->st_atim.tv_nsec);
    printf("Lasd file modification:     Sec=%ld     Nanosec=%ld\n",
           sb->st_mtim.tv_sec, sb->st_mtim.tv_nsec);
    printf("Last status change:         Sec=%ld     Nanosec=%ld\n",
           sb->st_ctim.tv_sec, sb->st_ctim.tv_nsec);
}

int main(char argc, char *argv[])
{
    struct stat sb;

    if ((argc < 2) || (strcmp(argv[1], "--help")) == 0) {
        fprintf(stderr, "%s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &sb) == -1) {
        fprintf(stderr, "stat failed! line: %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printfStat(&sb);

    exit(EXIT_SUCCESS);
}