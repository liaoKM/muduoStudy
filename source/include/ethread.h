#ifndef ETHREADHEADER
#define ETHREADHEADER
#include<memory>
#include<thread>
#include<functional>
#include"eventloop.h"

namespace muduo
{
//class EventLoop;
typedef std::function<void(EventLoop*)> ThreadInitCallback;

class EventLoopThread
{
    
public:
    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
    ~EventLoopThread();
    //std::weak_ptr<EventLoop> startLoop();
    EventLoop* getLoop() {return &loop_;}

private:
    void runInThread();
    EventLoop loop_;
    std::thread thread_;
    ThreadInitCallback callback_;
    
};
}



#endif