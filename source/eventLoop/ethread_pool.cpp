#include "../include/ethread_pool.h"
#include "../include/ethread.h"
#include "../include/eventloop.h"
#include <assert.h>
namespace muduo
{
    EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop)
        :baseLoop_(baseloop),
        started_(false),
        numThreads_(0),
        threads_(0),
        next_(0)
    {

    }

    EventLoopThreadPool::~EventLoopThreadPool()
    {
    }
    void EventLoopThreadPool::setThreadNum(int numThreads) 
    { 
        numThreads_ = numThreads; 
    }

    void EventLoopThreadPool::start(const Callback& cb)
    {
        assert(!started_);
        assert(numThreads_!=0);
        started_=true;

        for(int i=0;i<numThreads_;i++)
        {
            threads_.push_back(std::unique_ptr<EventLoopThread>(new EventLoopThread(cb)));
        }
        if (numThreads_ == 0 && cb)
        {
            cb(baseLoop_);
        }

    }

    EventLoop* EventLoopThreadPool::getNextLoop()
    {
        assert(started_);
        assert(!threads_.empty());

        // round-robin
        EventLoop* pLoop = (*threads_[next_%threads_.size()]).getLoop();
        next_=(next_+1)%threads_.size();

        return pLoop;
    }




}

