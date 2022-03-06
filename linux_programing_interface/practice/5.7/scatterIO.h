#ifndef _SACTTER_IO_H_
#include <unistd.h>

struct myiovec {
    void *iov_base;
    ssize_t iov_len;
};

ssize_t myreadv(int fd, const struct myiovec *iov, int iovcnt);
ssize_t mywritev(int fd, const struct myiovec *iov, int iovcnt);

#endif // !_SACTTER_IO_H_