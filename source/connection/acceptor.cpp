#include"../include/acceptor.h"
#include<unistd.h>
#include<fcntl.h>
#include<assert.h>
#include"../include/eventloop.h"
namespace muduo
{
Acceptor::Acceptor(EventLoop* loop, const sockaddr_in& listenAddr, bool reuseport)
    :loop_(loop),
    acceptSocket_(),
    acceptChannel_(loop, acceptSocket_.getFd()),
    listening_(false),
    idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    assert(idleFd_ >= 0);
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}
Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    ::close(idleFd_);
}
void Acceptor::listen()
{
    assert(loop_->isInLoopThread());
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
    assert(loop_->isInLoopThread());
    sockaddr_in peerAddr;
    int connfd=acceptSocket_.accept(&peerAddr);
    if(connfd>=0)
    {
        if(newConnCallback_)
        {
            newConnCallback_(connfd,peerAddr);
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        //todo log error
        if(errno==EMFILE)
        {
            ::close(idleFd_);
            idleFd_ = ::accept(acceptSocket_.getFd(), NULL, NULL);
            ::close(idleFd_);
            idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}


}