#ifndef BUFFERHEADER
#define BUFFERHEADER
#include<stddef.h>
#include<sys/types.h>
#include<vector>

namespace muduo
{
//todo 改进Buffer List<array>的形式
//size_append<1KB memcpy to Free node
//size_append>1KB unlink(inputBuffer->node) and link it to this
class Buffer
{
public:
    Buffer(size_t size=1024);
    void pop(size_t len);
    void push_back(void* const data,size_t len);
    void push_back(Buffer& inputBuf);
    void getContent(void*& oPointer,size_t& oLen);

    ssize_t readFd(int fd,int& oErrno);
    bool is_empty() const {return usedEnd_==usedStart_;}
    int getUsedSize() const {return usedEnd_-usedStart_;}
    int getFreeSize() const {return buffer_.size()-usedEnd_;}
    const char* getUsedPointer() const {return &(*buffer_.begin())+usedStart_;};
    const char* getFreePointer() const {return &(*buffer_.begin())+usedEnd_;};

private:
    void reset(){usedStart_=0;usedEnd_=0;}
    void checkWritable(size_t len);
    void makespace(size_t len);
    std::vector<char> buffer_;
    int usedStart_;
    int usedEnd_;


};
}


#endif

