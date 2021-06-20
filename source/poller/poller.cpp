#include "../include/poller.h"
#include "../include/EpollPoller.h"
#include "../include/eventloop.h"
#include "../include/channel.h"
#include <assert.h>
namespace muduo
{
    Poller::Poller(EventLoop* loop):
        ownerLoop_(loop)
    {
    }
    Poller::~Poller() = default;

    void Poller::assertInLoopThread() const
    {
        assert(ownerLoop_->isInLoopThread());
    }

    bool Poller::hasChannel(Channel* channel) const
    {
        assertInLoopThread();
        auto it = channels_.find(channel->fd());
        return it != channels_.end() && it->second == channel;
    }

    Poller* Poller::newDefaultPoller(EventLoop* loop)
    {
        /*if (::getenv("MUDUO_USE_POLL"))
        {
            return new PollPoller(loop);
        }
        else
        {
            return new EPollPoller(loop);
        }*/
        return new EpollPoller(loop);
    }


}
