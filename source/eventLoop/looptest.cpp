#include "../include/ethread_pool.h"
#include<thread>
int main()
{
    muduo::EventLoopThreadPool pool(nullptr);
    pool.initAndSetThreadNum(2);
    pool.start();
    std::this_thread::sleep_for(std::chrono::seconds(100));
}