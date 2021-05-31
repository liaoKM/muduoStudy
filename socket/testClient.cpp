#include"../include/socket.h"
#include<iostream>
#include<arpa/inet.h>
#include<string>

int main()
{
    muduo::Socket clientSock;
    sockaddr_in remoteAddr;
    remoteAddr.sin_family=AF_INET;
    remoteAddr.sin_port=8000;
    remoteAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    std::cout<<"accept...";
    clientSock.connect(&remoteAddr);
    std::cout<<"ok"<<std::endl;


    int conSockfd=clientSock.getFd();
    char* recvBuf=new char[1024];
    char* sendBuf=new char[1024];
    int len=recv(conSockfd,recvBuf,1024,0);
    recvBuf[len]='\0';
    
    std::cout<<std::string(recvBuf)<<std::endl;



    std::cout<<"ok"<<std::endl;
}