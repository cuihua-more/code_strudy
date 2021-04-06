#include "tlpi_hdr.h"
#include <ctype.h>
#include <fcntl.h>


int main(char argc, char *argv[])
{
    int fd;
    char *buf;
    int offset, length;
    ssize_t numRd, numWr;

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
            length = getLong(&argv[i][1], GN_ANY_BASE, argv[i]);
            buf = malloc(length);
            if (buf == NULL) {
                errExit("malloc");
            }

            numRd = read(fd, buf, length);
            if (numRd == -1) {
                errExit("read");
            }

            if (numRd == 0) {
                printf("%s: end-of-file\n", argv[i]);
            } else {
                for (int j = 0; j < numRd; j++) {
                    if (argv[i][0] == 'r') {
                        printf("%c", isprint((unsigned char)buf[j]) ? buf[j] : '?');
                    } else {
                        printf("%02x", (unsigned int)buf[j]);
                    }
                }
                printf("\n");
            }

            break;

        case 'w':   /* write string at currnet offset */
            numWr = write(fd, &argv[i][1], strlen(&argv[i][1]));
            if (numWr == -1) {
                errExit("write");
            }
            printf("%s: wrote %ld bytes\n", argv[i], (long)numWr);
            break;

        case 's':   /* Change file offset */
            offset = getLong(&argv[i][1], GN_ANY_BASE, argv[i]);

            if (lseek(fd, offset, SEEK_CUR) == -1) {
                errExit("lseek");
            }
            printf("%s: seek successed\n", argv[i]);
            break;

        default:

            break;
        }
    }

    exit(EXIT_SUCCESS);
}
