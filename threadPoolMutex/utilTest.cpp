#include"threadPool.h"
#include<atomic>
#include<iostream>
#include<unistd.h>

std::atomic<int> testSum(0);


void task()
{
    ++testSum;
    //std::cout<<testSum<<" ";
}


int main()
{
    muduo::ThreadPool thrPool(1024*1024,16);
    std::cout<<thrPool.queueSize()<<std::endl;
    std::cout<<testSum.load()<<std::endl;
    thrPool.start();
    for(int i=0;i<1024;i+=8)
    {
        thrPool.run(task);
        thrPool.run(task);
        thrPool.run(task);
        thrPool.run(task);

        thrPool.run(task);
        thrPool.run(task);
        thrPool.run(task);
        thrPool.run(task);
    }
    sleep(5);
    std::cout<<testSum.load()<<std::endl;

}