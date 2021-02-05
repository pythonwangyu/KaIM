//
// Created by ka on 2021/1/13.
//

#include "TimerQueue.h"
#include "KaTime.h"


using namespace KaNet;

TimerQueue::TimerQueue(EventLoop *loop)
:m_loop(loop)
{
}
TimerQueue::~TimerQueue() =default;

TimerSmartPtr TimerQueue::addTimer(const TimerCallback &cb, Timestamp when, int64_t interval, int repeatCount) {
    TimerSmartPtr timer(new Timer(cb,when,interval,repeatCount));
    insert(timer);
    return timer;
}

TimerSmartPtr TimerQueue::addTimer(TimerCallback &&cb, Timestamp when, int64_t interval) {
    TimerSmartPtr  timer(new Timer(std::move(cb),when,interval));
    insert(timer);
    return timer;
}
void TimerQueue::removeTimer(TimerSmartPtr &timer) {
    for(auto iter = m_timers.begin(); iter != m_timers.end(); ++iter)
    {
        if(iter->second == timer)
        {
            m_timers.erase(iter);
            break;
        }
    }
}

//
void TimerQueue::cancel(TimerSmartPtr &timer, bool off) {
    for(const auto & m_timer : m_timers)
    {
        if(m_timer.second == timer)
        {
            m_timer.second->Control(off);
            break;
        }
    }
}

//执行计时器队列的事件
void TimerQueue::doTimer() {
    Timestamp now(Timestamp::now());
    for(auto iter = m_timers.begin();iter!=m_timers.end();)
    {
        if(iter->second->exprivation() <= now)
        {
            iter->second->Run();
            if(iter->second->GetRepeatCount() == 0)
            {
                iter = m_timers.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        else
        {
            break;
        }
    }
}

void TimerQueue::insert(TimerSmartPtr &timer) {
    Timestamp when = timer->exprivation();
    m_timers.insert(Entry(when,timer));
}


//循环事件
LoopItemSmartPtr LoopEventList::addLoopEvent(const LoopFunction &cb,void* arg) {
    LoopItemSmartPtr item(new LoopItem(cb,arg));
    m_loopList.push_back(item);
    return item;
}

void LoopEventList::removeLoopEvent(LoopItemSmartPtr &itemPtr) {
    for(auto it = m_loopList.begin(); it != m_loopList.end();++it)
    {
        if(*it == itemPtr)
        {
            m_loopList.erase(it);
            break;
        }
    }
}


void LoopEventList::doLoopEvent() {
    for(auto & it : m_loopList)
    {
        it->Run();
    }
}





