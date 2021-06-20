#include"../include/EpollPoller.h"
#include"../include/channel.h"
#include <sys/epoll.h>
#include <unistd.h>
#include<assert.h>

namespace muduo
{
EpollPoller::EpollPoller(EventLoop* loop):Poller(loop),
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    buffSize(kInitEventListSize),
    eventsBuf_(new epoll_event[buffSize])
{
    //todo check epollfd
}
EpollPoller::~EpollPoller()
{
    ::close(epollfd_);
}
time_t EpollPoller::poll(int timeoutMs, ChannelList& oActiveChannels)
{
    int eventsNum=::epoll_wait(epollfd_,eventsBuf_.get(),buffSize,timeoutMs);
    if(eventsNum>0)
    {
        //todo log

        //events(return from wait)  --> oActiveChannels
        getActiveChannels(eventsNum,oActiveChannels);
        if(eventsNum==buffSize)
        {
            buffSize*=2;
            eventsBuf_.reset(new epoll_event[buffSize]);
        }
    }
    else if(eventsNum==0)
    {
        //todo log
    }
    else
    {
        //todo log error
    }
    return time(nullptr);
}
void EpollPoller::getActiveChannels(int numEvents,ChannelList& oActiveChannels) const
{
    for(int i=0;i<numEvents;i++)
    {
        epoll_event event=eventsBuf_.get()[i];
        Channel* c=static_cast<Channel*>(event.data.ptr);
        int fd=c->fd();
        assert(channels_.find(fd)!=channels_.end());
        assert(channels_.find(fd)->second==c);

        c->set_revents(event.events);
        oActiveChannels.push_back(c);
    }
}
void EpollPoller::addChannel(Channel* channel)
{
    int fd = channel->fd();
    assert(channels_.find(fd) == channels_.end());
    channels_[fd] = channel;
    update(EPOLL_CTL_ADD, channel);
    
}
void EpollPoller::updateChannel(Channel* channel)
{
    Poller::assertInLoopThread();
    int fd=channel->fd();
    assert(channels_.find(fd) != channels_.end());
    update(EPOLL_CTL_MOD, channel);
}
void EpollPoller::removeChannel(Channel* channel)
{
    Poller::assertInLoopThread();
    int fd = channel->fd();
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    channels_.erase(fd);
    update(EPOLL_CTL_DEL, channel);
}
void EpollPoller::update(int operation, Channel* channel)
{
    epoll_event event;
    event.events=channel->events();
    event.data.ptr=channel;
    int fd=channel->fd();
    int res=::epoll_ctl(epollfd_, operation, fd, &event);
    if(res<0)
    {
        //todo log error;
    }
}

}