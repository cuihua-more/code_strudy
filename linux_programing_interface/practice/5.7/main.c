#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include "scatterIO.h"
#include <string.h>

#define IOV_CNT     10

int main(char argc, char *argv[])
{
    int i;
    struct myiovec iov[IOV_CNT];
    int fd;

    if (argc < 2) {
        fprintf(stderr, "%s file \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd == -1 ) {
        fprintf(stderr, "create %s failed!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < IOV_CNT; i++) {
        iov[i].iov_base = malloc(3);
        if (iov[i].iov_base == NULL) {
            fprintf(stderr, "iov[%d] malloc failed!\n", i);
            goto malloc_fail;
        }
        iov[i].iov_len = 3;
        snprintf(iov[i].iov_base, 3, "%c\n", (char)i + 31);
    }

    if (mywritev(fd, iov, IOV_CNT) != IOV_CNT) {
        fprintf(stderr, "myWritev failed!\n");
        goto write_fail;
    }

    for (i = 0; i < IOV_CNT; i++) {
        memset(iov[i].iov_base, 0, iov[i].iov_len);
    }

    if (myreadv(fd, iov, IOV_CNT) != IOV_CNT) {
        fprintf(stderr, "myreadv failed!\n");
        goto write_fail;
    }

    for (i = 0; i < IOV_CNT; i++) {
        printf("%c\n", *(char *)(iov[i].iov_base));
        free(iov[i].iov_base);
    }

    close(fd);

    exit(EXIT_SUCCESS);

write_fail:
        close(fd);
malloc_fail:
    for (i--; i >= 0; i--) {
        free(iov[i].iov_base);
    }
    exit(EXIT_FAILURE);
}