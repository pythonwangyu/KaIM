//
// Created by ka on 2021/1/15.
//

#include "EventLoopThread.h"

#include <memory>
#include <utility>
#include "EventLoop.h"

using namespace KaNet;

EventLoopThread::EventLoopThread(ThreadInitCallback cb, const std::string &name)
:m_loop(nullptr),m_exiting(false),m_callback(std::move(cb))
{}

EventLoopThread::~EventLoopThread() {
    m_exiting = true;
    if(m_loop != nullptr)
    {
        m_loop->quit();
        m_thread->join();
    }
}

//注意这里返回的m_loop
EventLoop * EventLoopThread::startLoop() {
    //下面的线程创建之后可能立即执行，也可能延迟执行。
    m_thread = std::make_unique<std::thread>(std::bind(&EventLoopThread::threadFunc,this));
    {
        std::unique_lock<std::mutex> lock(m_mutex);     //自动释放锁
        while(m_loop == nullptr)
        {
            m_cond.wait(lock);          //唤醒Channel
        }
    }
    return m_loop;
}

void EventLoopThread::stopLoop() {
    if(m_loop != nullptr)
    {
        m_loop->quit();
    }
    m_thread->join();
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    if(m_callback)
    {
        m_callback(&loop);
    }
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_loop = &loop;             //唤醒条件产生
        m_cond.notify_all();        //若是执行了这个才执行到while(loop)?
    }
    loop.loop();        //该线程进入死循环
    m_loop = nullptr;
}
