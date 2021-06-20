#ifndef EPOLLHEADER
#define EPOLLHEADER
#include "../include/poller.h"
#include<sys/epoll.h>
#include<memory>
#include<array>
namespace muduo
{
class EpollPoller:public Poller
{
public:
    EpollPoller(EventLoop* loop);
    ~EpollPoller();
    time_t poll(int timeoutMs, ChannelList& oActiveChannels);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    void addChannel(Channel* channel);

private:
    static const int kInitEventListSize = 16;
    //static const char* operationToString(int op);
    void getActiveChannels(int numEvents,ChannelList& oActiveChannels) const;
    void update(int operation, Channel* channel);


    int epollfd_;
    int buffSize;
    std::unique_ptr<epoll_event> eventsBuf_;
};
}



#endif