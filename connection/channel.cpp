#include"../include/channel.h"
#include<assert.h>
#include"../include/eventloop.h"
namespace muduo
{

Channel::Channel(EventLoop* loop, int fd__)
  : loop_(loop),
    fd_(fd__),
    events_(0),
    revents_(0),
    index_(-1),
    logHup_(true),
    tied_(false),
    eventHandling_(false),
    addedToLoop_(false)
{
  
}

Channel::~Channel()
{
    assert(!eventHandling_);
    assert(!addedToLoop_);
    if (loop_->isInLoopThread())
    {
        assert(!loop_->hasChannel(this));
    }
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
  tie_ = obj;
  tied_ = true;
}

void Channel::update()
{
  if(addedToLoop_==false&&!isNoneEvent())
  {
    loop_->addChannel(this);
    addedToLoop_ = true;
  }
  else if(addedToLoop_==true&&!isNoneEvent())
  {
    loop_->updateChannel(this);
  }
  else if(addedToLoop_==true&&isNoneEvent())
  {
    loop_->removeChannel(this);
    addedToLoop_ = false;
  }
  //else addedToLoop_==false&&isNoneEvent() do nothing
  
}

/*void Channel::remove()
{
  assert(isNoneEvent());
  addedToLoop_ = false;
  loop_->removeChannel(this);
}*/

void Channel::handleEvent(time_t receiveTime)
{
  std::shared_ptr<void> guard;
  if (tied_)
  {
      guard = tie_.lock();
      if (guard)
      {
        handleEventWithGuard(receiveTime);
      }
  }
  else
  {
    handleEventWithGuard(receiveTime);
  }
}

void Channel::handleEventWithGuard(time_t receiveTime)
{
  eventHandling_ = true;
  if (revents_ & (POLLERR | POLLNVAL))
  {
    if (errorCallback_) errorCallback_();
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
  {
    if (readCallback_) readCallback_(receiveTime);
  }
  if (revents_ & POLLOUT)
  {
    if (writeCallback_) writeCallback_();
  }
  eventHandling_ = false;
}

}