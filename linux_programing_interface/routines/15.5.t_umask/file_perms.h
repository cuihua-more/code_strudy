#ifndef FILE_PERMS_H_
#define FILE_PERMS_H_

#include <sys/types.h>

#define FP_SPECIAL  1

char *filePermStr(mode_t perm, int flags);

#endif // !FILE_PERMS_H_