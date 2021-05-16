#ifndef THREADPOOLHEADER
#define THREADPOOLHEADER

#include <functional>
#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>

namespace muduo
{
typedef std::function<void ()> Task;

class ThreadPool
{
public:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    explicit ThreadPool(int taskQueMaxSize=1024,int threadNum=16);
    ~ThreadPool();
    void run(Task task);
    void stop();
    int queueSize();



private:
    bool checkFullInLock();
    void runInThread();
    Task take();

    std::mutex mutex;
    std::condition_variable notEmpty;
    std::condition_variable notFull;

    //std::string name;
    //Task threadInitCallBack;
    std::deque<Task> taskQue;
    std::vector<std::unique_ptr<std::thread>> threads;

    size_t maxQueueSize;
    bool running;

};




}

#endif