#ifndef _TLPI_HDR_H
#define _TLPI_HDR_H

#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <error.h>
#include <string.h>

#include "error_functions.h"

typedef enum
{
    FALSE,
    TRUE
} Boolean;

#define max(m, n) ((m) > (n) ? (m) : (n))
#define min(m, n) ((m) < (n) ? (m) : (n))



#endif // !_TLPI_HDR_H