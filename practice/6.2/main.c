#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

jmp_buf env;

static void saveenv(void)
{
    if (setjmp(env) == 0) {
        printf("saveenv");
    }
}

int main(char argc, char *argv[])
{
    saveenv();

    longjmp(env, 2);

    return 0;
}