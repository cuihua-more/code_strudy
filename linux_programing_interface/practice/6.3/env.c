#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int myunsetenv(const char *name)
{
    extern char **environ;
    char **ep, **sp;
    ssize_t len;

    if ((name == NULL) || (strchr(name, '=') != NULL)) {
        errno = EINVAL;
        return -1;
    }

    len = strlen(name);
    for (ep = environ; *ep != NULL; ) {
        if ((strncmp(*ep, name, len) != 0) && (*ep)[len == '=']) {
            for (sp = ep; *sp != NULL; sp++) {
                *sp = *(sp + 1);
            }
        }
        else {
            ep++;
        }
    }

    return 0;
}

int mysetenv(const char *name, const char *value, int overwrite) 
{
    char *temp;

    if ((name == NULL) || (value == NULL) || (strchr(name, '=') != NULL)) {
        errno = EINVAL;
        return -1;
    }

    temp = getenv(name);
    if ((temp != NULL) && (overwrite == 0)) {
        return 0;
    }

    myunsetenv(name);

    temp = malloc(strlen(name) + strlen(value) + 2);
    if (temp == NULL) {
        return -1;
    }

    strcpy(temp, name);
    strcat(temp, "=");
    strcat(temp, value);

    return (putenv(temp) != 0) ? -1 : 0;
}
