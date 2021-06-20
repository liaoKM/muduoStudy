#ifndef TCPSERVERHEADER
#define TCPSERVERHEADER
#include<unordered_set>
#include<memory>
#include<netinet/in.h>
#include<string>
#include<atomic>
#include"tcp_connection.h"
#include"ethread.h"

namespace muduo
{
class Acceptor;
class EventLoop;
class EventLoopThreadPool;
//class TcpConnection;

class TcpServer
{
public:
    enum Option
    {
        kNoReusePort,
        kReusePort,
    };
    TcpServer(EventLoop* loop,const sockaddr_in& listenAddr,Option option=kNoReusePort);
    ~TcpServer();

    const std::string& ipPort() const {return ipPort_;}
    EventLoop* getLoop() const{return loop_;}
    std::shared_ptr<EventLoopThreadPool> getThreadPool(){ return threadPool_; }

    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallback& cb){ threadInitCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback& cb){ connCallback = cb; }
    void setMessageCallback(const MessageCallback& cb){ messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb){ writeCompleteCallback_ = cb; }

    void start();

private:
    /// Not thread safe, but in loop
    void newConnection(int sockfd, const sockaddr_in& peerAddr);
    /// Thread safe.
    void removeConnection(const std::shared_ptr<TcpConnection>& conn);
    /// Not thread safe, but in loop
    void removeConnectionInLoop(const std::shared_ptr<TcpConnection>& conn);

    typedef std::unordered_set<std::shared_ptr<TcpConnection>> ConnectionSet;

    EventLoop* loop_;
    const std::string ipPort_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;
    ConnectionCallback connCallback;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    ThreadInitCallback threadInitCallback_;
    std::atomic<bool> started_;

    int nextConnId_;
    ConnectionSet connections_; 




};
}


#endif