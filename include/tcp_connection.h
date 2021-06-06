#ifndef CONNECTIONHEADER
#define CONNECTIONHEADER
#include<memory>
#include<atomic>
#include<functional>
#include<netinet/tcp.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<time.h>
#include"../include/socket.h"
#include"../include/buffer.h"

namespace muduo
{
class Channel;
class EventLoop;



class TcpConnection:public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop,int sockfd,const sockaddr_in localAddr,const sockaddr_in peerAddr);
    ~TcpConnection();
    EventLoop* getLoop() const { return loop_; }
    sockaddr_in localAddress() const { return localAddr_; }
    sockaddr_in peerAddress() const { return peerAddr_; }
    //Buffer* inputBuffer() { return &inputBuffer_; }
    //Buffer* outputBuffer(){ return &outputBuffer_; }

    bool isReading() const {return reading_;}
    bool isConnected() const {return state_==kConnected;}
    bool isDisConnected() const {return state_==kDisconnected;}

    void setCloseCallback(const CloseCallback& cb) { closeCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb){ messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb){ writeCompleteCallback_ = cb; }
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
    { highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark; }

    void send(void* const message, size_t len);
    void send(Buffer* message);  // this one will swap data
    void startRead();
    void stopRead();

    void connectEstablished();
    void shutdown(); // NOT thread safe, no simultaneous calling
    void forceClose();
    void forceCloseWithDelay(double seconds);
    void setTcpNoDelay(bool on);
private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    EventLoop* loop_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    const sockaddr_in localAddr_;
    const sockaddr_in peerAddr_;

    std::atomic<StateE> state_;
    std::atomic<bool> reading_;

    size_t highWaterMark_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;

    void handleRead(time_t receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInLoop(void* const message, size_t len);
    void shutdownInLoop();
    void forceCloseInLoop();
    void startReadInLoop();
    void stopReadInLoop();

};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
typedef std::function<void (const TcpConnectionPtr&,Buffer*,time_t)> MessageCallback;


}








#endif
