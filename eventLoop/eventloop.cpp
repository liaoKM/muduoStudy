#include"../include/eventloop.h"
#include<iostream>
namespace muduo
{
    EventLoop::EventLoop()
        :looping_(false),
        tied_(false),
        tiedThreadId_(),
        quit_(false)
    {

    }
    void EventLoop::loop()
    {
        if(tied_==false)
        {
            tied_=true;
            tiedThreadId_=std::this_thread::get_id();
        }
        else
        {
            assertInLoopThread();
        }

        assert(looping_==false);
        quit_=false;

        while(!quit_)
        {
            //todo
            std::cout<<"threadId:"<<tiedThreadId_<<"in loop"<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        

    }
    void EventLoop::quit()
    {
        quit_=true;
    }
}