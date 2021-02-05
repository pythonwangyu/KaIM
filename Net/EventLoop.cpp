#include "EventLoop.h"

#include <iostream>
#include <memory>
#include <sys/eventfd.h>

using namespace KaNet;

//使用这个线程局部变量来识别loop所属的线程
thread_local EventLoop* t_loopThisThead = nullptr;
//epoll_wait函数设定的返回值
const int KaPollerTimerMs = 1;

EventLoop::EventLoop()
        : m_looping(false),
          m_quit(false),
          m_eventHanding(false),
          m_callingPendingFunctors(false),
          m_threadId(std::this_thread::get_id()),
          m_TimerQueue(new TimerQueue(this)),
          m_interation(0L),
          m_currentActiveChannel(nullptr) {
    createWakeUpFd();
    m_wakeUpChannel = std::make_unique<KaChannel>(this,m_wakeupFd);
    m_Poller = std::make_unique<KaPoller>(this);
    if(t_loopThisThead)
    {
        std::cout << "another EventLoop exist in this thread" << std::endl;
    }
    else
    {
        t_loopThisThead = this;
    }
    m_wakeUpChannel->setReadCallback(std::bind(&EventLoop::handleRead,this));
    m_wakeUpChannel->enableReading();
}


EventLoop::~EventLoop() {
    m_wakeUpChannel->disableAll();
    m_wakeUpChannel->remove();
    ::close(m_wakeupFd);
    t_loopThisThead = nullptr;
}


void EventLoop::loop() {
    m_looping = true;
    m_quit = false;
    while(!m_quit)
    {
        m_TimerQueue->doTimer();            //处理定时器事件
        m_activeChannels.clear();           //清除激活的Channel
        m_pollReturnTime = m_Poller->poll(KaPollerTimerMs,&m_activeChannels);
        ++m_interation;
        m_eventHanding = true;
        for(const auto& it:m_activeChannels)
        {
            m_currentActiveChannel = it;
            m_currentActiveChannel->handleEvent(m_pollReturnTime);
        }
        m_currentActiveChannel = nullptr;
        m_eventHanding = false;
        doPendingFunctors();        //处理额外的函数
    }
    m_looping = false;
}

void EventLoop::quit() {
    m_quit = false;
}



/***********************************private function***************************************/

bool EventLoop::createWakeUpFd() {
    m_wakeupFd = ::eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
    if(m_wakeupFd < 0)
    {
        return false;
    }
    return true;
}

bool EventLoop::wakeUp() const {
    uint64_t  one = 1;
    int32_t n = ::write(m_wakeupFd,&one,sizeof(one));
    if(n != sizeof(one))
    {
        return false;
    }
    return true;
}


bool EventLoop::handleRead() const {
    uint64_t  one = 1;
    int32_t n = ::read(m_wakeupFd,&one,sizeof(one));
    if(n != sizeof(one))
    {
        return false;
    }
    return true;
}


//处理额外的函数
void EventLoop::doPendingFunctors() {
    m_callingPendingFunctors = true;
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        for(const auto& it:m_pendingFunctors)
        {
            it();
        }
        m_pendingFunctors.clear();
    }
    m_callingPendingFunctors = false;
}


bool EventLoop::updateChannel(KaChannel *channel) {
    if(channel->GetLoop() != this)
    {
        return false;
    }
    return m_Poller->updateChannel(channel);
}


void EventLoop::removeChannel(KaChannel *channel) {
    if(channel->GetLoop() != this)
    {
        return;
    }
    if(m_eventHanding)
    {
        //TODO：这里我还没有想好怎么做。
    }
    m_Poller->removeChannel(channel);
}

bool EventLoop::hasChannel(KaChannel *channel) {
    if (channel->GetLoop() != this)
    {
        return false;
    }
    return m_Poller->hasChannel(channel);
}