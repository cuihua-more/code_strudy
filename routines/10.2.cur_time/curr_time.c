#include "curr_time.h"

#define TIME_BUF    100

char * currTime(const char *format)
{
    static char buf[TIME_BUF];
    time_t t;
    size_t s;
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);
    if (tm == NULL) {
        return NULL;
    }

    s = strftime(buf, TIME_BUF, (format != NULL) ? format : "%c", tm);

    return (s == 0) ? NULL : buf;
}