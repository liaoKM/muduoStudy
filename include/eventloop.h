#ifndef EVENTLOOPHEADER
#define EVENTLOOPHEADER
#include<functional>
#include<vector>
#include<memory>
#include<thread>
#include<atomic>
#include<assert.h>
#include<mutex>

namespace muduo
{

class Channel;
class Poller;

class EventLoop
{
public:
    typedef std::function<void()> Func;

    EventLoop();

    //start loop, tie this to thread, and update information
    void loop();
    //stop loop
    void quit();
    void wakeup();

    /// Runs callback immediately in the loop thread.
    /// It wakes up the loop, and run the cb.
    /// If in the same loop thread, cb is run within the function.
    /// Safe to call from other threads.
    void runInLoop(const Func&  cb);
    /// Queues callback in the loop thread.
    /// Runs after finish pooling.
    /// Safe to call from other threads.
    void queueInLoop(const Func&  cv);

    void addChannel(Channel* channel);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);


    bool isInLoopThread() const { return tiedThreadId_ == std::this_thread::get_id(); }
    bool hasChannel(Channel* channel);

private:
    void assertInLoopThread() {assert(tiedThreadId_ == std::this_thread::get_id());}

    std::vector<Channel*> activeChannels_;
    Channel* currentActiveChannel_;
    std::unique_ptr<Poller> poller_;
    time_t pollReturnTime_;

    //waked up thread from poller.wait() when quit() is called.
    void handleWakeup();
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_; 

    std::atomic<bool> eventHandling_;
    std::atomic<bool> looping_;
    std::atomic<bool> tied_;
    std::thread::id tiedThreadId_;
    std::atomic<bool> quit_;
    int iteration_;

    std::mutex mutex_;
    std::vector<Func> pendingFunctors_;
    std::atomic<bool> calledPendingFunctors_;

};
}




#endif