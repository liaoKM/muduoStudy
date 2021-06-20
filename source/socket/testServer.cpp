#include"../include/socket.h"
#include<iostream>
#include<unistd.h>
int main()
{
    muduo::Socket serverSock;
    sockaddr_in localAddr;
    localAddr.sin_family=AF_INET;
    localAddr.sin_port=8000;
    localAddr.sin_addr.s_addr=INADDR_ANY;

    std::cout<<"bind Address...";
    serverSock.bindAddress(localAddr);
    std::cout<<"ok"<<std::endl;

    std::cout<<"listen...";
    serverSock.listen();
    std::cout<<"ok"<<std::endl;

    sockaddr_in remoteAddr;
    std::cout<<"accept...";
    int conSockfd=serverSock.accept(&remoteAddr);
    std::cout<<"ok"<<std::endl;

    std::cout<<"send...";
    send(conSockfd,"Welcome to server",21,0);
    std::cout<<"ok"<<std::endl;

    return 0;

}