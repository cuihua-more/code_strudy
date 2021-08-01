#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

extern char **environ;

#define SHELL_PATH      "/bin/sh"

int errPrintf(int line, char *format, ...)
{
    char buf[1024];
    va_list ap;

    memset(buf, 0, sizeof(buf));

    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);

    if (line != 0) {    
        snprintf(buf + strlen(buf), strlen(buf), "line = %d\n", line);
    }

    fputs(buf, stderr);
    exit(-1);
}

static int execShScrip(int argc, char *argv[], char *envp[])
{
    char **shArg = calloc(argc + 1, sizeof(char *));
    if (shArg == NULL) {
        return -1;
    }

    shArg[0] = SHELL_PATH;
    for (int i = 0; i < argc; i++) {
        shArg[i + 1] = argv[i];
    }

    execve(SHELL_PATH, shArg, envp);

    free(shArg);
}

int myExeclp(const char *filename, const char *arg, ...)
{
    /* build argv param */
    int argvSize = 100;
    char **argv = calloc(argvSize, sizeof(char *));
    if (argv == 0) {
        errPrintf(__LINE__, "calloc");
        return -1;
    }

    argv[0] = (char *)arg;
    int argc = 1;

    va_list argList;
    va_start(argList, arg);
    while (argv[argc - 1] != NULL) {
        if ((argc + 1) >= argvSize) {
            char **nargv;

            argvSize += 100;
            nargv = realloc(argv, sizeof(char *));
            if (nargv == NULL) {
                free(argv);
                errPrintf(__LINE__, "realloc");
            } else {
                argv = nargv;
            }
        }
        argv[argc] = va_arg(argList, char *);
        argc++;
    }
    va_end(argList);

    /* build env param*/
    int i;
    for (i = 0; environ[i] != NULL;) {
        i++;
    }

    char **envp = calloc(i = 1, sizeof(char *));
    if (envp == NULL) {
        free(argv);
        errPrintf(__LINE__, "calloc");
    }

    for (i = 0; environ[i] != NULL; i++) {
        envp[i] = strdup(environ[i]);
    }

    envp[i] = NULL;

    /* build Abs path */
    int fndEACCES = 0;
    int saveError;

    if (strchr(filename, '/') != NULL) {
        char *pathname = strdup(filename);
        execve(pathname, argv, envp);

        saveError = errno;
        if (errno == ENOEXEC) {
            execShScrip(argc, argv, envp);
        }
        free(pathname);
    } else {
        char *p = getenv("PATH");
        char *path = (p == NULL || strlen(p)) ? strdup(".") : strdup(p);

        char *pStart, *pEnd;

        pStart = path;
        char morePrefixes;

        while (morePrefixes) {
            pEnd = strchr(pStart, ':');
            if (pEnd == NULL) {
                pEnd = pStart + strlen(pStart);
            }

            char *pathName = malloc((pEnd - pStart) + strlen(filename) + 2);
            if (pStart == pEnd) {
                strcat(pathName, ".");
            } else {
                strncat(pathName, pStart, (pEnd - pStart));
            }

            strcat(pathName, "/");
            strcat(pathName, filename);

            if (*pEnd  == '\0') {
                morePrefixes = 0;
            } else {
                pStart = pEnd + 1;
            }

            execve(pathName, argv, envp);
            saveError = errno;
            if (errno == EACCES) {
                fndEACCES = 1;
            } else if (errno == ENOEXEC) {
                execShScrip(argc, argv, envp);
            }

             /* Avoid memory leaks, in case no execve() succeeds! */
            free(pathName);
        }
        free(path);
    }

     /* If we get here, execve() failed; clean up and return -1, ensuring
       that errno contains the value returned by (the last) execve() */

    free(argv);
    for (int j = 0; envp[j] != NULL; j++)
        free(envp[j]);
    free(envp);

    /* SUSv3 says that if any execve() failed with EACCES, then that
       is the error that should be returned by exec[lv]p() */

    errno = fndEACCES ? EACCES : saveError;
    return -1;
}

int main(char argc, char *argv[])
{
    myExeclp(argv[1], "123", "456");
    exit(EXIT_SUCCESS);
}
