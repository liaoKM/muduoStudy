#include "include/tcp_client.h"
#include "include/ethread.h"
#include "include/ethread_pool.h"
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>

using namespace muduo;

int numThreads = 0;
class EchoClient;
std::vector<std::unique_ptr<EchoClient>> clients;
int current = 0;


class EchoClient
{
public:
    EchoClient(EventLoop* loop, const sockaddr_in& listenAddr)
        : loop_(loop),
        client_(loop, listenAddr)
    {
        client_.setConnectionCallback(
            std::bind(&EchoClient::onConnection, this, std::placeholders::_1));
        client_.setMessageCallback(
            std::bind(&EchoClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        //client_.enableRetry();
    }

    void connect()
    {
        client_.connect();
    }
  // void stop();

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        //LOG_TRACE << conn->localAddress().toIpPort() << " -> "
        //    << conn->peerAddress().toIpPort() << " is "
        //    << (conn->connected() ? "UP" : "DOWN");

        if (conn->isConnected())
        {
            ++current;
            if (static_cast<size_t>(current) < clients.size())
            {
                clients[current]->connect();
            }
            //LOG_INFO << "*** connected " << current;
        }
        char* str="world\n";
        conn->send(str,6);
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, time_t time)
    {
        std::string msg(buf->getUsedPointer(),buf->getUsedSize());
        //LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << time;
        std::cout<<"recv"<<msg<<std::endl;
        if (msg.compare("quit\n")==0)
        {
            char* strBye="bye\n";
            conn->send(reinterpret_cast<void*>(strBye),4);
            conn->shutdown();
        }
        else if (msg.compare("shutdown\n")==0)
        {
            loop_->quit();
        }
        else
        {
            conn->send(buf);
        }
    }


    EventLoop* loop_;
    TcpClient client_;
};


int main()
{

    EventLoop loop;
    sockaddr_in serverAddr;
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=8000;
    serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    int n = 1;
    clients.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        clients.emplace_back(new EchoClient(&loop, serverAddr));
    }

    clients[current]->connect();
    loop.loop();

}
