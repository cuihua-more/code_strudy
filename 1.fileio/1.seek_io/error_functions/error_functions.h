#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

#ifdef __GNUC__
#define NORETURN    __attribute__ ((__noreturn__))
#else 
#define NORETURN
#endif // __GNUC__

void usageErr(const char *format, ...) NORETURN;

#endif // !ERROR_FUNCTIONS_H
