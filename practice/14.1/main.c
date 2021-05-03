#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

static void mkFile(char argc, char *argv[])
{
    long numFile, i;
    unsigned int n;
    int fd;
    char pathTemp[1024];
    char chr = 'x';

    numFile = atol(argv[3]);

    if (argc > 4) { /* srand */
        srand((unsigned)time(NULL));
        for (i = 0; i < numFile; i++) {
            n = rand();
            sprintf(pathTemp, "%s/x%d.txt", argv[2], n);
            fd = open(pathTemp, O_CREAT, S_IRUSR | S_IWUSR);
            if (fd == -1) {
                fprintf(stderr, "create %dtn file failed! line:%d\n", (int)i, __LINE__);
            }
            write(fd, &chr, 1);
            close(fd);
        }
    }
    else {
        for (i = 0; i < numFile; i++) {
            sprintf(pathTemp, "%s/x%d.txt", argv[2], (int)i);
            fd = open(pathTemp, O_CREAT, S_IRUSR | S_IWUSR);
            if (fd == -1) {
                fprintf(stderr, "create %dtn file failed! line:%d\n", (int)i, __LINE__);
            }
            write(fd, &chr, 1);
            close(fd);
        }
    }

    exit(EXIT_SUCCESS);
}

static void rmFile(char argc, char *argv[])
{
    DIR *dir;
    struct dirent *dirent;

    dir = opendir(argv[2]);
    if (dir == NULL) {
        fprintf(stderr, "open dir %s failed! line:%d", argv[2], __LINE__);
        exit(EXIT_FAILURE);
    }

    dirent = readdir(dir);
    while (dirent != NULL) {
        
    }

    if (argc > 3) { /* srand */

    }
    else {

    }

    exit(EXIT_SUCCESS);
}

int main(char argc, char *argv[])
{

    if (argc < 2 || (strcmp(argv[1], "--help") == 0)) {
cmd:
        fprintf(stderr, "%s mk parh numFile [srand] \n", argv[0]);
        fprintf(stderr, "%s rm path [srand] \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "mk") == 0) {
        mkFile(argc, argv);
    }
    else if (strcmp(argv[1], "rm") == 0) {
        rmFile(argc, argv);
    }
    else {
        goto cmd;
    }

    exit(EXIT_SUCCESS);
}