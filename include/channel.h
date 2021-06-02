#ifndef CHANNELHEADER
#define CHANNELHEADER

#include<functional>
#include<time.h>
#include<memory>
#include<string>
#include <poll.h>

#define NONEEVENT 0
#define READEVENT (POLLIN | POLLPRI)
#define WRITEEVENT POLLOUT

namespace muduo
{
class EventLoop;

class Channel
{
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(time_t)> ReadEventCallback;

    Channel(EventLoop* loop,int fd);
    ~Channel();
    void handleEvent(time_t receiveTime);

    void setReadCallback(ReadEventCallback cb)
    { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb)
    { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb)
    { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb)
    { errorCallback_ = std::move(cb); }

    /// Tie this channel to the owner object managed by shared_ptr,
    /// prevent the owner object being destroyed in handleEvent.
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; } // used by pollers
    // int revents() const { return revents_; }
    bool isNoneEvent() const { return events_ == NONEEVENT; }

    void enableReading() { events_ |= READEVENT; update(); }
    void disableReading() { events_ &= ~READEVENT; update(); }
    void enableWriting() { events_ |= WRITEEVENT; update(); }
    void disableWriting() { events_ &= ~WRITEEVENT; update(); }
    void disableAll() { events_ = NONEEVENT; update(); }//remove channel
    bool isWriting() const { return events_ & WRITEEVENT; }
    bool isReading() const { return events_ & READEVENT; }

    // for Poller
    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    // for debug
    std::string reventsToString() const;
    std::string eventsToString() const;

    void doNotLogHup() { logHup_ = false; }

    EventLoop* ownerLoop() { return loop_; }
    //void remove();

private:
    void update();
    void handleEventWithGuard(time_t receiveTime);

    EventLoop* loop_;
    const int  fd_;
    int        events_;
    int        revents_; // it's the received event types of epoll or poll
    int        index_; // used by Poller.
    bool       logHup_;

    std::weak_ptr<void> tie_;
    bool tied_;
    bool eventHandling_;
    bool addedToLoop_;
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};
}
#endif