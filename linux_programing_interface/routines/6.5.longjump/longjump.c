#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

static jmp_buf env;

static void f2(void)
{
    longjmp(env, 2);
}

static void f1(char argc)
{
    if (argc == 1) {
        longjmp(env, 1);
    }
    f2();
}

int main(char argc, char *argv[])
{

    switch (setjmp(env))
    {
    case 0:
        printf("Calling f1() after inital setjmp()\n");
        f1(argc);
        break;

    case 1:
        printf("We jumpd back from f1()\n");
        break;

    case 2:
        printf("We jumpd back from f2()\n");
        break;

    default:
        break;
    }
    exit(EXIT_SUCCESS);
}