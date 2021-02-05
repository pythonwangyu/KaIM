//
// Created by ka on 2021/1/15.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

#include <sstream>

using namespace KaNet;

EventLoopThreadPool::EventLoopThreadPool()
    :m_baseLoop(nullptr),m_started(false),m_numberThreads(0),m_next(0)
{
    std::cout << "EventLoopThreadPool" << std::endl;
}

EventLoopThreadPool::~EventLoopThreadPool() = default;


void EventLoopThreadPool::init(EventLoop *baseLoop, int numThreads) {
    m_numberThreads = numThreads;
    m_baseLoop = baseLoop;
}

//启动线程池
void EventLoopThreadPool::start(const ThreadInitCallback &cb) {
    if((m_baseLoop == nullptr) || m_started)
        return ;
    m_started = true;

    for(int i = 0; i < m_numberThreads;++i)             //核心循环
    {
        char buf[128] = {0};
        snprintf(buf,sizeof buf,"%s%d",m_name.c_str(),i);
        //线程Loop的名字
        std::unique_ptr<EventLoopThread> t(new EventLoopThread(cb,buf));
        //将独占式的线程智能指针和循环插入到vector中。
        m_loops.push_back(t->startLoop());
        m_threads.push_back(std::move(t));
    }
    if(m_numberThreads == 0 && cb)
    {
        cb(m_baseLoop);         //只有主线程的函数
    }
}

void EventLoopThreadPool::stop() {
    for(auto &it:m_threads)
    {
        it->stopLoop();
    }
}

EventLoop * EventLoopThreadPool::getNextLoop() {
//    m_baseLoop->assertInLoopThread();
    if(!m_started)
        return nullptr;

    EventLoop* loop = m_baseLoop;
    if(!m_loops.empty())
    {
        loop = m_loops[m_next];             //轮循的获取下一个Loop
        ++m_next;
        if(static_cast<size_t>(m_next) >= m_loops.size())
        {
            m_next = 0;
        }
    }
    return loop;
}



EventLoop * EventLoopThreadPool::GetLoopForHash(size_t HashCode) {
  //  m_baseLoop->assertInLoopThread();
    EventLoop* loop = m_baseLoop;
    if(!m_loops.empty())
    {
        loop = m_loops[HashCode % m_loops.size()];
    }
    return loop;
}


std::vector<EventLoop *> EventLoopThreadPool::GetAllThreadLoop() {
//    m_baseLoop->assertInLoopThread();
    if(m_loops.empty())
    {
        return std::vector<EventLoop*>(1,m_baseLoop);
    }
    else
    {
        m_loops;
    }
}


std::string EventLoopThreadPool::info() const {
    std::stringstream ss;
    ss << "print thread id info " << std::endl;
    for(size_t i = 0; i < m_loops.size(); i++)
    {
        ss << i << ": threadId = " << m_loops[i]->getThreadID() << std::endl;
    }
    return ss.str();
}