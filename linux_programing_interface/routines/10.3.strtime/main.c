#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <locale.h>
#include <string.h>

#define SBUF_SIZE   1000

/*
 * /demo "9:58:57pm 23 April 2021" "%I:%M:%S%p %d %b %Y" "%F %T"
 * ./demo "9:58:57pm 23 April 2021" "%I:%M:%S%p %d %b %Y"
 */
int main(char argc, char *argv[])
{
    struct tm tm;
    time_t time;
    char sbuf[SBUF_SIZE];
    char *fmt;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s input-data-time in-format [out-fotmat]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (setlocale(LC_ALL, "") == NULL) {
        fprintf(stderr, "call setlocale failed! line:%d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    memset((void *)&tm, 0, sizeof(tm));
    if (strptime(argv[1], argv[2], &tm) == NULL) {
        fprintf(stderr, "call strptime failed! line:%d \n", __LINE__);
        exit(EXIT_FAILURE);
    }

    tm.tm_isdst = -1;

    printf("calendar time (second since Epoch): %ld\n", (long)mktime(&tm));

    fmt = (argc > 3) ? argv[3] : "%H:%M:%S %A, %d %B %Y %Z";
    if (strftime(sbuf, SBUF_SIZE, fmt, &tm) == 0) {
        fprintf(stderr, "call strftime failed! line:%d \n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("strptime() yields: %s\n", sbuf);

    exit(EXIT_SUCCESS);
}