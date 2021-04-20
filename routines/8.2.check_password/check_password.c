#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <shadow.h>
#include <errno.h>
#include <crypt.h>

int main(char argc, char *argv[])
{
    long lnmax;
    char *username, *password, *encrypted, *p;
    int len;
    struct passwd   *pwd;
    struct spwd     *spwd;

    lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    if (lnmax == -1) {
        lnmax = 255;
    }

    username = malloc(lnmax);
    if (username == NULL) {
        fprintf(stderr, "malloc %ld bytes failed! file:%s line:%d\n", lnmax, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("UserName: ");
    fflush(stdout);
    if (fgets(username, lnmax, stdin) == NULL) {
        fprintf(stderr, "\nUserName input error!\n");
        exit(EXIT_FAILURE);
    }

    len = strlen(username);
    if (username[len - 1] == '\n') {
        username[len - 1] = 0;
    }

    pwd = getpwnam(username);
    if (pwd == NULL) {
        fprintf(stderr, "cound't get password record!\n");
        exit(EXIT_FAILURE);
    }

    spwd = getspnam(username);
    if (spwd == NULL && errno == EACCES) {
        fprintf(stderr, "The caller does not have permission to access the shadow password file!\n");
        exit(EXIT_FAILURE);
    }

    if (spwd != NULL) {
        pwd->pw_passwd = spwd->sp_pwdp;
    }

    password = getpass("PassWord: ");

    /* Encrypt password */
    /* link option -lcrypt */
    encrypted = crypt(password, pwd->pw_passwd);
    /* after encrypt password, we should clear password */
    for (p = password; *p != 0;) {
        *p++ = 0;
    }

    if (encrypted == NULL) {
        fprintf(stderr, "crypt error!\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(pwd->pw_passwd, encrypted)) {
        printf("Incorrect password!\n");
        exit(EXIT_FAILURE);
    }

    printf("Successfully authenticated: UID=%ld\n", (long)pwd->pw_uid);
    exit(EXIT_SUCCESS);
}