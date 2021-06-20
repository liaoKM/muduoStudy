#ifndef ACCEPTORHEADER
#define ACCEPTORHEADER
#include"../include/socket.h"
#include"../include/channel.h"
#include<netinet/in.h>

namespace muduo
{
class EventLoop;

class Acceptor
{
public:
    typedef std::function<void (int sockfd, sockaddr_in iNetAddr)> NewConnectionCallback;
    Acceptor(EventLoop* loop,const sockaddr_in& listenAddr, bool reuseport);
    ~Acceptor();
    void setNewConnCallback_(const NewConnectionCallback& cb){newConnCallback_=cb;}
    void listen();
    bool isListening()const{return listening_;}


private:
    void handleRead();
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnCallback_;
    bool listening_;
    int idleFd_;//处理错误
    

};




}










#endif