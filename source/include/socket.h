#ifndef SOCKETHEADER
#define SOCKETHEADER
#include<netinet/tcp.h>
#include<netinet/in.h>
namespace muduo
{

class Socket
{    
public:
    Socket(const Socket& s)=delete;
    void operator=(const Socket& s)=delete;

    explicit Socket(int sockfd):sockfd(sockfd) {}
    explicit Socket() {sockfd=::socket(PF_INET,SOCK_STREAM,0);}

    ~Socket();
    int getFd() const {return sockfd;}
    bool getTcpInfo(tcp_info* oInfo);

    void bindAddress(const sockaddr_in& localAddr);
    void connect(sockaddr_in* iRemoteAddr);
    
    void listen();
    int accept(sockaddr_in* oRemoteAddr);
    
    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);



private:
    int sockfd;

};


}



#endif