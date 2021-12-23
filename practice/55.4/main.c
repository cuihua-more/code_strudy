#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <myDef.h>

#define TEMP1_PATH  "temp1.txt"
#define TEMP2_PATH  "temp2.txt"

int main(char argc, char *argv[])
{
    int fd1, fd2;

    //fd1 = open(TEMP1_PATH, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);
    //if (fd1 == -1) {
    //    
    //}
    errPrintf(__FILE__, __LINE__, strerror(errno), "HelloWord");
}