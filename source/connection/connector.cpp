#include"../include/connector.h"
#include"../include/channel.h"
#include"../include/eventloop.h"
#include<unistd.h>

namespace muduo
{
Connector::Connector(EventLoop* loop,const sockaddr_in& serverAddr)
:loop_(loop),
serverAddr_(serverAddr),
connect_(false),
state_(kDisconnected),
retryDelayMs_(kInitRetryDelayMs)
{
    //todo log
}

Connector::~Connector()
{
    //todo log
}

void Connector::start()
{
    connect_=true;
    loop_->runInLoop(std::bind(&Connector::startInLoop,this));
}

void Connector::startInLoop()
{
    assert(loop_->isInLoopThread());
    assert(state_==kDisconnected);
    if(connect_)
    {
        connect();
    }
    else
    {
        //debug log error
    }

}

void Connector::stop()
{
    connect_=false;
    loop_->runInLoop(std::bind(&Connector::stopInLoop,this));
}

void Connector::stopInLoop()
{
    assert(loop_->isInLoopThread());
    if(state_ == kConnecting)//::connect以完成，channel已经建立
    {
        state_=kDisconnected;
        channel_->disableAll();//关闭传输
        ::close(channel_->fd());
    }

}

void Connector::restartInLoop()
{
    assert(loop_->isInLoopThread());
    state_=kDisconnected;
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}

void Connector::connect()
{
    //todo socket 封装
    int sockfd=::socket(serverAddr_.sin_family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    int ret = ::connect(sockfd, reinterpret_cast<sockaddr*>(&serverAddr_),sizeof(sockaddr_in));
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno)
    {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
        connecting(sockfd);
        break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
        retry(sockfd);
        break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
        //todo log syserr
        ::close(sockfd);
        break;

        default:
        //todo log syserr
        ::close(sockfd);
        // connectErrorCallback_();
        break;
    }

}

void Connector::connecting(int fd)
{
    state_=kConnecting;
    assert(channel_.get()==nullptr);
    //state_=kConnected;
    channel_.reset(new Channel(loop_,fd));
    channel_->setWriteCallback(std::bind(&Connector::handleWrite,this));//可写触发，连接完成
    channel_->setErrorCallback(std::bind(&Connector::handleError,this));
    channel_->enableWriting();
}

void Connector::handleWrite()
{
    //LOG_TRACE << "Connector::handleWrite " << state_;

    if (state_ == kConnecting)
    {
        //连接建立，取消fd的write监听
        int fd=channel_->fd();
        channel_->disableAll();

        //todo getsocketerror
        //if(err)
        //{
        //    retry(fd);
        //}
        //else
        {
            state_=kConnected;
            if(connect_)
                newConnCallback_(fd);
            else
                ::close(fd);
            
        }

    }
    else
    {
        // what happened?
        assert(state_ == kDisconnected);
    }
}

void Connector::handleError()
{

}

void Connector::retry(int fd)
{
    
}

}