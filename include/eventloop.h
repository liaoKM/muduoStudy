#ifndef EVENTLOOPHEADER
#define EVENTLOOPHEADER
#include<functional>
#include<vector>
#include<memory>
#include<thread>
#include<atomic>
#include<assert.h>

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

    bool isInLoopThread() const { return tiedThreadId_ == std::this_thread::get_id(); }
private:
    void assertInLoopThread() {assert(tiedThreadId_ == std::this_thread::get_id());}
    //std::vector<Channel*> activeChannels_;
    //Channel* currentActiveChannel_;
    //std::unique_ptr<Poller> poller_;
    //waked up thread from poller.wait() when quit() is called.
    //std::unique_ptr<Channel> wakeupChannel_; 
    std::atomic<bool> looping_;
    std::atomic<bool> tied_;
    std::thread::id tiedThreadId_;
    std::atomic<bool> quit_;




};
}




#endif