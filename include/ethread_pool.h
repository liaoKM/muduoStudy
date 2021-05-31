#ifndef EPOOLHEADER
#define EPOOLHEADER

#include <memory>
#include <vector>
#include <functional>

namespace muduo
{

class EventLoop;
class EventLoopThread;



class EventLoopThreadPool
{
public:
    typedef std::function<void (std::weak_ptr<EventLoop>)> Callback;

    EventLoopThreadPool(std::shared_ptr<EventLoop> baseloop);
    ~EventLoopThreadPool();
    void initAndSetThreadNum(int numThreads,const Callback& cb=Callback());
    void start(const Callback& cb=Callback());

    /////// valid after calling start()/////////////
    /// round-robin
    std::weak_ptr<EventLoop> getNextLoop();
    /// with the same hash code, it will always return the same EventLoop
    //EventLoop* getLoopForHash(size_t hashCode);
    bool started() const { return started_; }


private:
    std::shared_ptr<EventLoop> baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    //std::vector<EventLoop*> loops_;

};
}
#endif