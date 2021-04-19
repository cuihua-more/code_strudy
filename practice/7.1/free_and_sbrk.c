#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define MAX_ALLOCS  100000

int main(char argc, char *argv[])
{
    char *ptr[MAX_ALLOCS];
    int numAllocs, sizeBlock;
    int freeStep, freeMin, freeMax;
    int i, j;

    if ((argc < 3) && (argv[1] != NULL) && (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s num-allocs block-size [step [min [max]]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    numAllocs = atol(argv[1]);
    sizeBlock = atol(argv[2]);

    freeStep    = (argc > 3) ? atol(argv[3]) : 1;
    freeMin     = (argc > 4) ? atol(argv[4]) : 1;
    freeMax     = (argc > 5) ? atol(argv[5]) : numAllocs;

    if (freeMax > numAllocs) {
        fprintf(stderr, "free-max > num-allocs\n");
        exit(EXIT_SUCCESS);
    }

    printf("Inital program break:                   %10p\n", sbrk(0));

    printf("Allocting %d*%d bytes\n", numAllocs, sizeBlock);
    
    for (i = 0; i < numAllocs; i++) {
        ptr[i] = (char *)malloc(sizeBlock);
        if (ptr[i] == NULL) {
            fprintf(stderr, "malloc %d failed\n", i);
        }
        printf("After malloc %d counts, program break:  %10p\n", i, sbrk(0));
    }

    printf("Program block is now:                   %10p\n", sbrk(0));

    printf("Freeing blocks from %d to %d in step of %d\n", freeMin, freeMax, freeStep);
    for (j = freeMin - 1; j < freeMax; j+= freeStep){
        free(ptr[j]);
    }

    printf("Afrer free(), program break is %10p\n", sbrk(0));

    exit(EXIT_SUCCESS);
}