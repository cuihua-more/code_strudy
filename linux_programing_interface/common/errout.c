#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

void errPrintf(char *file, int line, char *err, char *fmt,...)
{
    va_list ap;
    char buf[4096];

    memset((char *)buf, 0, sizeof(buf));

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    if (file != NULL) {
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " file is %s ", file);
    }
    if (line != 0) {
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " line is %d ", line);
    }
    if (err != NULL) {
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " %s", err);
    }
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " \n");

    write(STDERR_FILENO, buf, strlen(buf));

    exit(EXIT_FAILURE);
}