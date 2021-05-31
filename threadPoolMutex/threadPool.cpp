#include"../include/threadPool.h"
#include"assert.h"
namespace muduo{//namespace ThreadPool{

ThreadPool::ThreadPool(int taskQueMaxSize,int threadNum)
    :mutex(),
    notEmpty(),
    notFull(),
    taskQue(taskQueMaxSize),
    threads(threadNum),
    //name(nameArg),
    maxQueueSize(taskQueMaxSize),
    running(false)
{

}
void ThreadPool::start()
{
    running=true;
    for(int i=0;i<threads.size();i++)
    {
        threads[i].reset(new std::thread(&ThreadPool::runInThread,this));
    }
}

bool ThreadPool::checkFullInLock()
{
    assert(mutex.try_lock()==false);

    return taskQue.size()>=maxQueueSize;
}

void ThreadPool::runInThread()
{
    try
    {
        // if(threadInitCallback)
        // {
        //     threadInitCallback();
        // }
        while(running)
        {
            Task task=take();
            if(task)
            {
                task();
            }
        }
    }
    // catch (const Exception& ex)
    // {
    //     fprintf(stderr, "exception caught in ThreadPool : thread %d\n", std::this_thread::get_id());
    //     fprintf(stderr, "reason: %s\n", ex.what());
    //     fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
    //     abort();
    // }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool : thread %d\n", std::this_thread::get_id());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool : thread %d\n", std::this_thread::get_id());
        throw; // rethrow
    }
    
}

Task ThreadPool::take()
{
    std::unique_lock<std::mutex> lg(mutex);
    while(running&&taskQue.empty())
    {
        notEmpty.wait(lg);
    }
    Task ret;
    if(!running)
    {
        return ret;
    }
    ret=taskQue.front();
    taskQue.pop_front();
    notFull.notify_one();
    return ret;
}
void ThreadPool::run(Task task)
{
    std::unique_lock<std::mutex> lg(mutex);
    while(checkFullInLock()&&running)
    {
        notFull.wait(lg);
    }
    if(!running)
    {
        return;
    }
    taskQue.push_back(std::move(task));
    notEmpty.notify_one();
}
int ThreadPool::queueSize()
{
    std::unique_lock<std::mutex> lg(mutex);
    return taskQue.size();
}

void ThreadPool::stop()
{
    std::unique_lock<std::mutex> lg(mutex);
    running=false;
    notEmpty.notify_all();
    notFull.notify_all();
    for(std::unique_ptr<std::thread>& thread:threads)
    {
        if(thread->joinable())
            thread->join();
    }
}

ThreadPool::~ThreadPool()
{
  if (running)
  {
    stop();
  }
}
}//}