#include"../include/tcp_server.h"
#include"../include/acceptor.h"
#include"../include/eventloop.h"
#include"../include/ethread_pool.h"
#include<memory>


namespace muduo
{
TcpServer::TcpServer(EventLoop* loop,const sockaddr_in& listenAddr,Option option)
    :loop_(loop),
    //dodo ipPort_()
    acceptor_(new Acceptor(loop,listenAddr,option==kReusePort)),
    threadPool_(new EventLoopThreadPool(loop_)),
    nextConnId_(1),
    started_(false)
{
    acceptor_->setNewConnCallback_(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    assert(loop_->isInLoopThread());
    //LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";

    for (const TcpConnectionPtr& item : connections_)
    {
        //TcpConnectionPtr conn(item);
        //connections_.erase(item);
        item->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, item));
    }
}

void TcpServer::setThreadNum(int numThreads)
{
    assert(0 <= numThreads);
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
    bool expcet=false;
    if (started_.compare_exchange_strong(expcet,true))
    {
        threadPool_->start(threadInitCallback_);
        assert(!acceptor_->isListening());
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

void TcpServer::newConnection(int sockfd, const sockaddr_in& peerAddr)//应该放在acceptor_中，逻辑割裂
{
    assert(loop_->isInLoopThread());
    EventLoop* ioLoop=threadPool_->getNextLoop();//threadPool应该是单例模式 通过ThreadPool::getInst()->getNextLoop()获取
    sockaddr_in localAddr;
    socklen_t addrlen=sizeof(sockaddr_in);
    ::getsockname(sockfd,reinterpret_cast<sockaddr*>(&localAddr),&addrlen);
    TcpConnectionPtr conn(new TcpConnection(ioLoop,sockfd,localAddr,peerAddr));
    connections_.insert(conn);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    // FIXME: unsafe
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    assert(loop_->isInLoopThread());// in mainReactor
    //LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
    //        << "] - connection " << conn->name();
    size_t n = connections_.erase(conn);
    (void)n;
    assert(n == 1);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));//run in subReactor
}

}