#include "tlpi_hdr.h"
#include <ctype.h>
#include <fcntl.h>


int main(char argc, char *argv[])
{
    int fd;
    char *buf;
    int offset, length;

    if ((argc < 3) || (strcmp(argv[1], "--help") == 0)) {
        usageErr("%s file { r<length> | R<length> | w<string> | s<offset> }...\n", argv[0]);
    }

    fd = open(argv[1], O_RDWR | O_CREAT | O_CLOEXEC, S_IRWXU);
    if (fd == -1) {
        printf("%s open failed!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    for (int i = 2; i++; i < argc) {
        switch (argv[i][0])         {
        case 'r':
        case 'R':
            length = atoi(argv[i][1]);
            break;

        case 'w':
            break;

        case 's':
            offset = atol(argv[i][1]);
            break;

        default:
            break;
        }
    }

    exit(EXIT_SUCCESS);
}
