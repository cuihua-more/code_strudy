#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <stdarg.h>

#define    XATTR_SIZE   10240

static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-x] file...\n", progName);
    exit(EXIT_FAILURE);
}

static void errExit(const char *format,...) 
{
    char buf[1024];
    va_list ap;

    va_start(ap, format);
    vsnprintf(buf, 1024, format, ap);
    va_end(ap);

    fprintf(stderr, "Error: %s\n", buf);

    exit(EXIT_FAILURE);
}

int main(char argc, char *argv[])
{
    int opt;
    char hexDispaly = 0;
    int i, j;
    char name[XATTR_SIZE], value[XATTR_SIZE];
    ssize_t listen, valuelen;
    int ns;

    while ((opt = getopt(argc, argv, "x")) != -1) {
        switch (opt) {
            case 'x': hexDispaly = 1; break;
            case '?': usageError(argv[0]);  
        }
    }

    for (i = optind; i < argc; i++) {
        listen = listxattr(argv[i], name, XATTR_SIZE);
        if (listen == -1) {
            errExit("listattr line: %d", __LINE__);
        }

        printf("%s\n", argv[i]);

        for (ns = 0; ns < listen; ns += strlen(&name[ns]) +1) {
            printf("    name=%s; ", &name[ns]);
            valuelen = getxattr(argv[i], &name[ns], value, XATTR_SIZE);
            if (valuelen == -1) {
                errExit("getxattr line: %d\n", __LINE__);
            } else if (!hexDispaly) {
                printf("value=%.*s\n", (int)valuelen, value);
            } else {
                printf("value=");
                for (j = 0; j < valuelen; j++) {
                    printf("%02x", (unsigned int)value[j]);
                }
                printf("\n");
            }
        }
    }

    exit(EXIT_SUCCESS);
}