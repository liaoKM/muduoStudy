#include"../include/socket.h"
#include <unistd.h>

namespace muduo
{
    Socket::~Socket()
    {
        ::close(sockfd);
    }
    void Socket::bindAddress(const sockaddr_in& localAddr)
    {
        ::bind(sockfd,(sockaddr*)&localAddr,sizeof(sockaddr_in));
    }

    void Socket::listen()
    {
        ::listen(sockfd,SOMAXCONN);
    }

    int Socket::accept(sockaddr_in* oRemoteAddr)
    {
        socklen_t len=static_cast<socklen_t>(sizeof(sockaddr_in));
        int clientSockfd=::accept(sockfd,(sockaddr*)oRemoteAddr,&len);
        return clientSockfd;
    }

    void Socket::connect(sockaddr_in* iRemoteAddr)
    {
        ::connect(sockfd,(sockaddr*) iRemoteAddr,sizeof(sockaddr_in));
    }

    void Socket::shutdownWrite()
    {
        ::shutdown(sockfd,SHUT_WR);
    }

    void Socket::setTcpNoDelay(bool on)
    {
        int optval = on ? 1 : 0;
        ::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,&optval, static_cast<socklen_t>(sizeof optval));
    }
    void Socket::setReuseAddr(bool on)
    {
        int optval = on ? 1 : 0;
        ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&optval, static_cast<socklen_t>(sizeof optval));
    }
    void Socket::setReusePort(bool on)
    {
        int optval = on ? 1 : 0;
        ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT,&optval, static_cast<socklen_t>(sizeof optval));
    }
    void Socket::setKeepAlive(bool on)
    {
        int optval = on ? 1 : 0;
        ::setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,&optval, static_cast<socklen_t>(sizeof optval));
    }
}