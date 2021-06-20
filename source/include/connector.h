#ifndef CONNECTORHEADER
#define CONNECTORHEADER
#include<netinet/in.h>
#include<atomic>
#include<memory>
#include<functional>
namespace muduo
{

class Channel;
class EventLoop;

class Connector
{
public:
    typedef std::function<void (int sockfd)> NewConnectionCallback;
    Connector(EventLoop* loop,const sockaddr_in& serverAddr);
    ~Connector();
    void setNewConnCallback(const NewConnectionCallback& cb){newConnCallback_=cb;}
    void start();
    void restartInLoop();
    void stop();
    const sockaddr_in& serverAddr() const {return serverAddr_;}
private:
    enum States {kDisconnected,kConnecting,kConnected};
    static const int kMaxRetryDelayMs = 30*1000;
    static const int kInitRetryDelayMs = 500;
    void startInLoop();
    
    void stopInLoop();
    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

    EventLoop* loop_;
    sockaddr_in serverAddr_;
    std::atomic<bool> connect_;
    std::atomic<States> state_;
    std::unique_ptr<Channel> channel_;
    NewConnectionCallback newConnCallback_;
    int retryDelayMs_;


};


}




#endif