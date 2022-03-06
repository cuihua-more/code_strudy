#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_LIZE    4096

static pthread_once_t dirOnce = PTHREAD_ONCE_INIT;
static pthread_once_t baseOnce = PTHREAD_ONCE_INIT;
static pthread_key_t dirKey, baseKey;


static void __errPrintf(int line, const char *format, ...)
{
    char buf[1024];

    memset(buf, 0, sizeof(buf));

    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);

    if (line != 0) {
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " line = %d\n", line);
    }

    fprintf(stderr, "%s", buf);

    fflush(stderr);
}

#define errPrintf(s, ...) __errPrintf(__LINE__, s, ##__VA_ARGS__)

static int judgePath(char *from, char *to)
{
    int i = 0;
    char flag = 0;
    char *tempFrom = from;
    char *tempTo = to;

    if ((from == NULL) || (to == NULL)) {
        return -1;
    }

    while (*tempFrom != 0) {
        if ((*tempFrom == '/') && (flag == 1)) {
            tempFrom++;
            continue;
        }
        *tempTo = *tempFrom;
        if (*tempFrom == '/') {
            flag = 1;
        }
        else {
            flag = 0;
        }
        tempFrom++;
        tempTo++;
        i++;
    }

    if (to[i - 1] == '/') {
        to[i - 1] = 0;
    }

    return 0;
}

static void createDirKeyFunc(void *buf)
{
    free(buf);
}

static void createDirKey(void)
{
    int s;

    s = pthread_key_create(&dirKey, createDirKeyFunc);

    if (s != 0) {
        errPrintf("pthread_key_create failed!");
    }
}

static char *myDirname(char *pathname)
{
    int s;
    char *buf;
    int fd;
    int len;

    s = pthread_once(&dirOnce, createDirKey);
    if (s != 0) {
        errPrintf("pthread_once failed!");
    }

    buf = pthread_getspecific(dirKey);
    if (buf == NULL) {
        buf = malloc(BUF_LIZE);
        if (buf == NULL) {
            errPrintf("malloc failed!");
        }

        s = pthread_setspecific(dirKey, buf);
        if (s != 0) {
            errPrintf("pthread_setspecific faild!");
        }
    }

    memset(buf, 0, BUF_LIZE);
    if (pathname == NULL) {
        strcat(buf, ".");
    } else if (strcmp(pathname, "/") == 0) {
        strcat(buf, "/");
    } else if (strcmp(pathname, "..") == 0) {
        strcat(buf, "..");
    } else {
        if (judgePath(pathname, buf) != 0) {
            return NULL;
        }
        fd = open(buf, O_RDONLY);
        if (fd == 0) {
            return NULL;
        }
        close(fd);
        len = strlen(buf);
        len--;
        while (len >= 0) {
            if (buf[len] == '/') {
                buf[len] = 0;
                return buf;
            }
            buf[len] = 0;
            len--;
        }
    }

    if (len < 0) {
        memset(buf, 0, BUF_LIZE);
        strcat(buf, ".");
    }
    return buf;
}

static void createBaseKeyFunc(void *buf)
{
    free(buf);
}

static void createBaseKey(void)
{
    int s;

    s = pthread_key_create(&baseKey, createBaseKeyFunc);
    if (s != 0) {
        errPrintf("pthread_key_create failed!\n");
    }
}

static char *myBasename(char *pathname)
{
    int s;
    char *buf;
    int fd;
    int len;
    char *temp;

    s = pthread_once(&baseOnce, createBaseKey);
    if (s != 0) {
        errPrintf("pthread_once failed!");
    }

    buf = pthread_getspecific(baseKey);
    if (buf == NULL) {
        buf = malloc(BUF_LIZE);
        if (buf == NULL) {
            errPrintf("malloc failed!");
        }

        s = pthread_setspecific(baseKey, buf);
        if (s != 0) {
            errPrintf("pthread_setspcific failed!");
        }
    }

    memset(buf, 0, BUF_LIZE);
    if (strcmp(pathname, "/") == 0) {
        strcat(buf, "/");
    } else if (strcmp(pathname, "..") == 0) {
        strcat(buf, "..");
    } else if (strcmp(pathname, ".") == 0) {
        strcat(buf, ".");
    } else {
        if (judgePath(pathname, buf) != 0) {
            return NULL;
        }
        fd = open(buf, O_RDONLY);
        if (fd == 0) {
            return NULL;
        }
        close(fd);
        len = strlen(buf);
        len--;
        while (len >= 0) {
            if (buf[len] == '/') {
                temp = &buf[len];
                break;
            }
            len--;
        }
        if (len < 0) {
            memset(buf, 0, BUF_LIZE);
            strcat(buf, pathname);
        } else {
            temp++;
            strcpy(buf, temp);
        }
        
    }

    return buf;
}

int main(char argc, char *argv[])
{
    char *buf = NULL;

    if ((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s filename \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    buf = myDirname(argv[1]);
    if (buf == NULL) {
        errPrintf("get %s dir failed!", argv[1]);
    } else {
        printf("dir: %s \n", buf);
        printf("buf addr is 0x%lx \n", (long)buf);
    }

    buf = myBasename(argv[1]);
    if (buf == NULL) {
        errPrintf("get %s base failed!", argv[1]);
    } else {
        printf("base: %s \n", buf);
        printf("buf addr is 0x%lx \n", (long)buf);
    }

    buf = myDirname(argv[1]);
    if (buf == NULL) {
        errPrintf("get %s dir failed!", argv[1]);
    } else {
        printf("dir: %s \n", buf);
        printf("buf addr is 0x%lx \n", (long)buf);
    }

    buf = myBasename(argv[1]);
    if (buf == NULL) {
        errPrintf("get %s base failed!", argv[1]);
    } else {
        printf("base: %s \n", buf);
        printf("buf addr is 0x%lx \n", (long)buf);
    }
    exit(EXIT_SUCCESS);
}
