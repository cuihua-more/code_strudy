#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "curr_time.h"

int main(char argc, char *argv[])
{
    char *time;

    time = currTime((argc > 1) ? argv[1] : NULL);

    if (time == NULL) {
        fprintf(stderr, "get time failed!\n");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", time);

    exit(EXIT_SUCCESS);
}