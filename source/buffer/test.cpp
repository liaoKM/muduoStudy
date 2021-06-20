#include "../include/buffer.h"
#include<iostream>
using namespace std;
void printContent(muduo::Buffer& buffer)
{
    void* content;
    size_t contentLen;
    buffer.getContent(content,contentLen);
    for(int i=0;i<contentLen;i++)
    {
        cout<<((char*)content)[i]<<" ";
    }
    cout<<endl;
}
int main()
{
    muduo::Buffer buffer(5);
    char input[3]={'a','b','c'};
    printContent(buffer);


    buffer.push_back(input,3);// abc ...
    printContent(buffer);
    buffer.push_back(input,3);// enlarge abcabc
    printContent(buffer);
    buffer.pop(3);//...abc
    printContent(buffer);
    buffer.push_back(input,3);//move and append
    printContent(buffer);
}