#include "include/tcp_server.h"
#include "include/ethread.h"
#include "include/ethread_pool.h"
#include <netinet/in.h>
#include<arpa/inet.h>

#include<iostream>
#include<unistd.h>

using namespace muduo;

int numThreads = 0;


class EchoServer
{
public:
    EchoServer(EventLoop* loop, const sockaddr_in& listenAddr)
        : loop_(loop),
        server_(loop, listenAddr)
    {
        server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        server_.setThreadNum(numThreads);
    }

    void start()
    {
        server_.start();
    }
    // void stop();

    private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        /*LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");
        LOG_INFO << conn->getTcpInfoString();*/
        char* str="hello\n";
        conn->send(str,6);
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, time_t time)
    {
        std::string msg(buf->getUsedPointer(),buf->getUsedSize());
        std::cout<<"recv"<<msg<<std::endl;
        if (msg.compare("exit\n")==0)
        {
            char* str="bye\n";
            conn->send(str,4);
            conn->shutdown();
        }
        if (msg == "quit\n")
        {
            loop_->quit();
        }
        conn->send(buf);
    }

  EventLoop* loop_;
  TcpServer server_;
};

int main()
{
  //LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
  //LOG_INFO << "sizeof TcpConnection = " << sizeof(TcpConnection);
    numThreads=1;

    EventLoop loop;
    sockaddr_in listenAddr;
    listenAddr.sin_family=AF_INET;
    listenAddr.sin_port=8000;
    listenAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    EchoServer server(&loop, listenAddr);

    server.start();

    loop.loop();
}
