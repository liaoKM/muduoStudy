#include "../include/ethread.h"
#include"../include/eventloop.h"

namespace muduo
{
    EventLoopThread::EventLoopThread(const ThreadInitCallback& cb)
        :loop_(),
        thread_(),
        callback_(cb)
    {
        thread_=std::thread(&EventLoopThread::runInThread,this);
    }
    // std::weak_ptr<EventLoop> EventLoopThread::startLoop()
    // {
    //     //thread_=std::thread(&EventLoop::loop,ploop_.get());
    //     thread_=std::thread(&EventLoopThread::runInThread,this);
    //     return ploop_;
    // }
    void EventLoopThread::runInThread()
    {
        if(callback_)
        {
            callback_(&loop_);
        }
        loop_.loop();
    }
    EventLoopThread::~EventLoopThread()
    {
        loop_.quit();
        if(thread_.joinable())
        {
            thread_.join();
        }
    }
}