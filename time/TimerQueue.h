//
// Created by ka on 2021/1/13.
//

#ifndef KANET_TIMERQUEUE_H
#define KANET_TIMERQUEUE_H
#include <set>
#include <list>
#include "TimeStamp.h"
#include "../Net/KaChannel.h"
#include "../Net/KaCallBack.h"
#include "../Net/EventLoop.h"



//TODO , 自己定义简单的最小堆或者红黑树来实现计数器的优化，这里我直接使用了stl中的set容器来完成这个
//计时器可以看做是一个函数触发器，这个触发器是由事件戳所限制的。

namespace KaNet{

    //前置声明
    class EventLoop;
    class Timer;
    class LoopItem;

    typedef std::shared_ptr<Timer>                  TimerSmartPtr;
    typedef std::pair<Timestamp,TimerSmartPtr>      Entry;
    typedef std::set<Entry>                         TimerList;
    typedef std::pair<TimerSmartPtr,int64_t>        ActivalTimer;
    typedef std::set<ActivalTimer>                  ActivalTimerSet;
    class TimerQueue {
    public:
        TimerQueue(EventLoop* loop);
        ~TimerQueue();

        //no copy attribute
        TimerQueue(const TimerQueue& obj) = delete;
        TimerQueue& operator=(const TimerQueue& obj)= delete;

        //添加计时器到计时器队里中
        TimerSmartPtr addTimer(const TimerCallback& cb,Timestamp when,int64_t interval,int repeatCount);
        TimerSmartPtr addTimer(TimerCallback&& cb,Timestamp when,int64_t interval);

        void removeTimer(TimerSmartPtr& timer);
        void cancel(TimerSmartPtr& timer,bool off);

        void doTimer();
        
    private:
        void insert(TimerSmartPtr& timer);

    private:
        EventLoop*      m_loop;             //计时器所属的循环
        TimerList       m_timers;           //基于set的计时器堆排序
    };


/***************************** 每次循环事件 *****************************************/


    typedef std::shared_ptr<LoopItem> LoopItemSmartPtr;
    typedef std::list<LoopItemSmartPtr> LoopItemList;

    class LoopEventList{
    public:
        LoopEventList() = default;
        ~LoopEventList() = default;

        //添加一个事件到循环事件里面
        LoopItemSmartPtr addLoopEvent(const LoopFunction& cb,void* arg);

        void removeLoopEvent(LoopItemSmartPtr& itemPtr);

        void doLoopEvent();

    private:
        LoopItemList m_loopList;
    };
}// end of namespace KaNet

#endif //KANET_TIMERQUEUE_H