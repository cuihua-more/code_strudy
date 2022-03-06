#ifndef _SVMUTEX_H_
#define _SVMUTEX_H_

#define ERROUT_FILENAME

#ifdef ERROUT_FILENAME
void errPrintf(int line, char *file, const char *fmt, ...);
#define ERROUT(...)  errPrintf(__LINE__, __FILE__, __VA_ARGS__)
#else
void errPrintf(int line, const char *fmt, ...);
#define ERROUT(...)  errPrintf(__LINE__, __VA_ARGS__)
#endif


int mutextGet(int semId, unsigned short type);
int mutexPut(int semId, unsigned short type);

#endif
