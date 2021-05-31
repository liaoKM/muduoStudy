#include "../include/ethread.h"
#include"../include/eventloop.h"

namespace muduo
{
    EventLoopThread::EventLoopThread(const ThreadInitCallback& cb)
        :ploop_(nullptr),
        thread_(),
        callback_(cb)
    {
        ploop_=std::shared_ptr<EventLoop>(new EventLoop());
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
            callback_(ploop_);
        }
        ploop_->loop();
    }
    EventLoopThread::~EventLoopThread()
    {
        if(ploop_.get()!=nullptr)
        {
            ploop_->quit();
        }
        if(thread_.joinable())
        {
            thread_.join();
        }
    }
}