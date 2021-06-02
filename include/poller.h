#ifndef POLLERHEADER
#define POLLERHEADER
#include<vector>
#include<map>
#include<time.h>
#include<memory>

namespace muduo
{
class Channel;
class EventLoop;

class Poller
{
public:
    typedef std::vector<Channel*> ChannelList;
    Poller(EventLoop* loop);
    virtual ~Poller();
    virtual time_t poll(int timeoutMs, ChannelList& oActiveChannels) = 0;
    virtual void addChannel(Channel* channel) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    virtual bool hasChannel(Channel* channel) const;
    static Poller* newDefaultPoller(EventLoop* loop);
    void assertInLoopThread() const;
protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channels_;

private:
    EventLoop* ownerLoop_;

};
}




#endif