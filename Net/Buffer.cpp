//
// Created by ka on 2021/1/14.
//

#include "Buffer.h"
#include <sys/uio.h>
using namespace KaNet;

const char Buffer::CRLF[] = "\r\n";

const size_t Buffer::Prepend;
const size_t Buffer::InitSize;

int32_t Buffer::readFd(int fd, int *Errno) {
    char extrabuf[65536];
    const size_t writable = writableBytes();
    struct iovec vec[2];
    vec[0].iov_base = begin() + m_writeIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    const int iovcnt = (writable < sizeof extrabuf) ? 2:1;
    const ssize_t n = ::readv(fd,vec,iovcnt);
    if(n <= 0)
    {
        *Errno = errno;
    }
    else if(static_cast<size_t>(n) <= writable)
    {
        m_writeIndex += n;
    }
    else
    {
        m_writeIndex = m_buffer.size();
        append(extrabuf,n-writable);
    }
    return n;
}