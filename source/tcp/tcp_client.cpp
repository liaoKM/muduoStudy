#include"../include/tcp_client.h"
#include"../include/eventloop.h"
#include"../include/connector.h"
#include"../include/tcp_connection.h"
#include<functional>
namespace muduo
{


TcpClient::TcpClient(EventLoop* loop,const sockaddr_in& serverAddr)
    :loop_(loop),
    connector_(new Connector(loop,serverAddr)),
    retry_(false),
    connect_(true),
    nextConnId_(1)

{
    connector_->setNewConnCallback(std::bind(&TcpClient::newConnection,this,std::placeholders::_1));

    //todo log info
}
TcpClient::~TcpClient()
{
    //关闭connection 或 connector
    std::lock_guard<std::mutex> guard(mutex_);
    if(connection_)
    {
        assert(loop_==connection_->getLoop());
        //connection建立时已注册close回调，不用处理
        //CloseCallback cb = std::bind(&muduo::removeConnection, loop_, std::placeholders::_1);
        //conn->setCloseCallback(cb);

    }
    else
    {
        connector_->stop();
    }
}

void TcpClient::connect()
{
    connect_ = true;
    connector_->start();
}
void TcpClient::disconnect()
{
    connect_ = false;
    std::lock_guard<std::mutex> guard(mutex_);
    if(connection_)
    {
        connection_->shutdown();
    }
}
void TcpClient::stop()
{
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
    assert(loop_->isInLoopThread());
    sockaddr_in localAddr;
    sockaddr_in peerAddr;
    socklen_t addrlen=sizeof(sockaddr_in);
    ::getsockname(sockfd,reinterpret_cast<sockaddr*>(&localAddr),&addrlen);
    ::getpeername(sockfd,reinterpret_cast<sockaddr*>(&peerAddr),&addrlen);
    TcpConnectionPtr conn(new TcpConnection(loop_,
                                            sockfd,
                                            localAddr,
                                            peerAddr));

    //conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
        std::bind(&TcpClient::removeConnection, this, std::placeholders::_1)); // FIXME: unsafe
    {
        std::lock_guard<std::mutex> guard(mutex_);
        connection_ = conn;
    }
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    assert(loop_->isInLoopThread());
    assert(loop_ == conn->getLoop());

    {
        std::lock_guard<std::mutex> guard(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if (retry_ && connect_)
    {
        //LOG_INFO << "TcpClient::connect[" << name_ << "] - Reconnecting to "
        //        << connector_->serverAddress().toIpPort();
        connector_->restartInLoop();
    }
}

}