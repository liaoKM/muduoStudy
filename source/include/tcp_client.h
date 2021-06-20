#ifndef TCPCLIENTHEADER
#define TCPCLIENTHEADER
#include<memory>
#include<atomic>
#include<mutex>
#include<netinet/in.h>
#include"tcp_connection.h"
namespace muduo
{
class Connector;
//class EventLoop;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient
{
public:
    TcpClient(EventLoop* loop,const sockaddr_in& serverAddr);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr getTcpConnection() {std::lock_guard<std::mutex> guard(mutex_);return connection_;}
    EventLoop* getLoop() const { return loop_; }
    bool retry() const { return retry_; }
    void enableRetry() { retry_ = true; }

    void setConnectionCallback(const ConnectionCallback& cb) { connCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }


private:
    void newConnection(int fd);//not tcp_connection
    void removeConnection(const TcpConnectionPtr& conn);
    EventLoop* loop_;
    ConnectorPtr connector_;
    ConnectionCallback connCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    std::atomic<bool> retry_;
    std::atomic<bool> connect_;
    int nextConnId_;
    std::mutex mutex_;
    TcpConnectionPtr connection_;



};



}



#endif