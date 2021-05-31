#ifndef ETHREADHEADER
#define ETHREADHEADER
#include<memory>
#include<thread>
#include<functional>

namespace muduo
{
class EventLoop;

class EventLoopThread
{
    typedef std::function<void(std::weak_ptr<EventLoop>)> ThreadInitCallback;
public:
    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
    ~EventLoopThread();
    //std::weak_ptr<EventLoop> startLoop();
    std::weak_ptr<EventLoop> getLoop() const {return ploop_;}

private:
    void runInThread();
    std::shared_ptr<EventLoop> ploop_;
    std::thread thread_;
    ThreadInitCallback callback_;
    
};
}



#endif