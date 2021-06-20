#include"../include/buffer.h"
#include<assert.h>
#include<unistd.h>
#include<sys/uio.h>
#include <errno.h>
namespace muduo
{

Buffer::Buffer(size_t size):buffer_(size,0),usedStart_(0),usedEnd_(0)
{

}

ssize_t Buffer::readFd(int fd,int& oErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = getFreeSize();
    vec[0].iov_base = &(*buffer_.begin())+usedEnd_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    // when there is enough space in this buffer, don't read into extrabuf.
    // when extrabuf is used, we read 128k-1 bytes at most.
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = readv(fd, vec, iovcnt);
    if(n<0)
    {
        oErrno=errno;
    }
    else if(n<writable)
    {
        usedEnd_+=n;
    }
    else
    {
        usedEnd_=buffer_.size();
        push_back(extrabuf,n-writable);
    }
    return n;
}

void Buffer::pop(size_t len)
{
    usedStart_+=len;
    if(is_empty())
    {
        reset();
    }
}

void Buffer::push_back(void* const data,size_t len)
{
    checkWritable(len);
    std::copy((char*)data,(char*)data+len,buffer_.begin()+usedEnd_);
    usedEnd_+=len;
}

void Buffer::checkWritable(size_t len)
{
    if(usedEnd_+len<=buffer_.size())
    {
        return;
    }
    else
    {
        makespace(len);
    }
}

void Buffer::makespace(size_t len)
{
    assert(usedEnd_+len>buffer_.size());
    if(usedEnd_-usedStart_+len<buffer_.size())//整理到头部，不扩容
    {
        auto head=buffer_.begin();
        std::copy(head+usedStart_,head+usedEnd_,head);
    }
    else//扩容
    {
        buffer_.resize(usedEnd_+len);
    }
}

void Buffer::getContent(void*& oPointer,size_t& oLen)
{
    oPointer=&(*(buffer_.begin()+usedStart_));
    oLen=usedEnd_-usedStart_;
}

}