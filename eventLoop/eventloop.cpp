#include"../include/eventloop.h"
#include"../include/poller.h"
#include"../include/channel.h"
#include<iostream>
#include<sys/eventfd.h>
#include<unistd.h>

namespace muduo
{
    const int kPollTimeoutMs=10000;
    EventLoop::EventLoop()
        :looping_(false),
        tied_(false),
        poller_(Poller::newDefaultPoller(this)),
        wakeupFd_(eventfd(0, EFD_NONBLOCK)),
        wakeupChannel_(new Channel(this,wakeupFd_)),
        tiedThreadId_(),
        quit_(false),
        eventHandling_(false),
        iteration_(0),
        calledPendingFunctors_(true),
        currentActiveChannel_(nullptr)
    {
        wakeupChannel_->setReadCallback(std::bind(handleWakeup,this));
        wakeupChannel_->enableReading();
    }
    void EventLoop::loop()
    {
        if(tied_==false)
        {
            tied_=true;
            tiedThreadId_=std::this_thread::get_id();
        }
        else
        {
            assertInLoopThread();
        }

        assert(looping_==false);
        quit_=false;

        while(!quit_)
        {
            //epoll wait
            activeChannels_.clear();
            pollReturnTime_ = poller_->poll(kPollTimeoutMs, activeChannels_);
            ++iteration_;
            calledPendingFunctors_ = false;
            //todo log

            //handling;
            eventHandling_ = true;
            for (Channel* channel : activeChannels_)
            {
                currentActiveChannel_ = channel;
                currentActiveChannel_->handleEvent(pollReturnTime_);
            }
            currentActiveChannel_ = NULL;
            eventHandling_ = false;

            //taskqueue in loop
            std::vector<Func> tempFucnList;
            {
                std::unique_lock<std::mutex> lockguard(mutex_);
                tempFucnList.swap(pendingFunctors_);
                calledPendingFunctors_ = true;
            }
            for(auto& func:tempFucnList)
            {
                func();
            }
            
        }
        

    }
    void EventLoop::wakeup()
    {
        int64_t buf=1;
        int size=write(wakeupFd_, &buf, sizeof(int64_t));
        if(size!=sizeof(int64_t))
        {
            //todo log error
        }
    }
    void EventLoop::quit()
    {
        quit_=true;
        if(!isInLoopThread())
        {
            wakeup();
        }
    }

    void EventLoop::addChannel(Channel* channel)
    {
        poller_->addChannel(channel);
    }
    void EventLoop::updateChannel(Channel* channel)
    {
        poller_->updateChannel(channel);
    }
    void EventLoop::removeChannel(Channel* channel)
    {
        poller_->removeChannel(channel);
    }

    bool EventLoop::hasChannel(Channel* channel)
    {
        poller_->hasChannel(channel);
    }

    void EventLoop::runInLoop(const Func& cb)
    {
        if(isInLoopThread())
        {
            cb();
        }
        else
        {
            queueInLoop(cb);
        }
    }

    void EventLoop::queueInLoop(const Func&  cb)
    {
        assert(!isInLoopThread());
        std::unique_lock<std::mutex> lockguard(mutex_);
        pendingFunctors_.push_back(cb);
        if (calledPendingFunctors_)
        {
            wakeup();
        }
        
    }
    void EventLoop::handleWakeup()
    {
        uint64_t buf = 1;
        ssize_t n = read(wakeupFd_, &buf, sizeof buf);
        //todo log
    }
}