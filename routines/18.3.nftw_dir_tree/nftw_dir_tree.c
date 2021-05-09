#define _XOPEN_SOURCE   600
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ftw.h>

static void usageErr(const char *progName, const char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s\n", msg);
    }

    fprintf(stderr, "Usage: %s [-d] [-m] [-p] [directory-path]\n", progName);
    fprintf(stderr, "\t-d Use FTW_DEPTH flag\n");
    fprintf(stderr, "\t-m Use FTW_MOUNT flag\n");
    fprintf(stderr, "\t-p Use FTW_PHYS flag \n");
    exit(EXIT_FAILURE);
}

static  int dirTree (const char *filename, const struct stat *status, 
                    int flag, struct FTW *info)
{
    switch ((status->st_mode & S_IFMT)) {
        case S_IFREG : printf("-"); break;
        case S_IFDIR : printf("d"); break;
        case S_IFCHR : printf("c"); break;
        case S_IFBLK : printf("b"); break;
        case S_IFLNK : printf("l"); break;
        case S_IFIFO : printf("p"); break;
        case S_IFSOCK : printf("s"); break;
        default : printf("?"); break;
    }

    printf(" %s ",
                (flag == FTW_D)     ? "D    " :
                (flag == FTW_DNR)   ? "DNR  " :
                (flag == FTW_DP)    ? "DP   " :
                (flag == FTW_F)     ? "F    " :
                (flag == FTW_NS)    ? "NS   " :
                (flag == FTW_SL)    ? "SL   " :
                (flag == FTW_SLN)   ? "SLN  " : "  ");

    if (flag != FTW_NS) {
        printf("%7ld", (long)status->st_ino);
    } else {
        printf("    ");
    }

    printf("%*s", 4 * info->level, " ");
    printf("%s\n", &filename[info->base]);
    return 0;
}

int main(char argc, char *argv[])
{
    int opt;
    int flags;

    while ((opt = getopt(argc, argv, "dmp")) != -1) {
        switch (opt) {
            case 'd': flags |= FTW_DEPTH; break;
            case 'm': flags |= FTW_MOUNT; break;
            case 'p': flags |= FTW_PHYS;  break;
            default: usageErr(argv[0], NULL); break;
        }
    }

    if (argc > optind +1) {
        usageErr(argv[0], NULL);
    }

    if (nftw((argc > optind) ? argv[optind] : ".", dirTree, 10, flags) != 0) {
        fprintf(stderr, "ntfw failed! line:%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_FAILURE);
}