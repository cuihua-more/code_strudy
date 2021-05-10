#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>

static char *myRealpath(const char *pathname, char *resolved_path)
{
    char tempLink[PATH_MAX], tempcwd[PATH_MAX];
    int rdNum;
    char flags;

    if (resolved_path == NULL) {
        resolved_path = malloc(PATH_MAX);
        if (resolved_path == NULL) {
            fprintf(stderr, "malloc %d bytes failed! line: %d\n", PATH_MAX, __LINE__);
            exit(EXIT_FAILURE);
        }
        flags = 1;
    }

    rdNum = readlink(pathname, tempLink, PATH_MAX);
    if (rdNum == -1) {
        fprintf(stderr, "readlink failed! line: %d\n", __LINE__);
        if (flags == 1) {
            free(resolved_path);
        }
        exit(EXIT_FAILURE);
    }
    tempLink[rdNum] = 0;

    if(getcwd(tempcwd, PATH_MAX) == NULL) {
        fprintf(stderr, "getcwd failed! line: %d\n", __LINE__);
        if (flags == 1) {
            free(resolved_path);
        }
        exit(EXIT_FAILURE);
    }

    sprintf(resolved_path, "%s/%s", tempcwd, tempLink);

    return resolved_path;
}

int main(char argc, char *argv[])
{
    char temp[PATH_MAX];
    char *p;

    if (argc < 2 || (strcmp(argv[1], "--help")) == 0) {
        fprintf(stderr, "%s file [-n]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc > 2) {
        p = myRealpath(argv[1], NULL);
    } else {
        p = myRealpath(argv[1], temp);
    }

    if (p == NULL) {
        fprintf(stderr, "myRealpath failed! line:%d \n", __LINE__);
    } else {
        printf("%s\n", p);
    }

    if (argc > 2) {
        free(p);
    }

    exit(EXIT_SUCCESS);
}