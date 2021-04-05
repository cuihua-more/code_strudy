#include "error_functions.h"
#include <stdarg.h>
#include "tlpi_hdr.h"

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

