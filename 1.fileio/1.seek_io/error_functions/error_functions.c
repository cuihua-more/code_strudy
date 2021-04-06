#include "error_functions.h"
#include <stdarg.h>
#include "tlpi_hdr.h"

static long
terminate(Boolean useExit3)
{
    char *s;

    s = getenv("EF_DUMPCORE");

     if (useExit3) {
        exit(EXIT_FAILURE);
    } else {
        _exit(EXIT_FAILURE);
    }
}

void usageErr(const char *format, ...)
{
    va_list argList;

    fflush(stdout);

    fprintf(stderr, "Usage: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fflush(stderr);

    exit(EXIT_FAILURE);
}

void errExit(const char *format, ...)
{
    va_list ap;

    fflush(stdout);

    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);

    fflush(stderr);

    //terminate(TRUE);

    exit(EXIT_FAILURE);
}
