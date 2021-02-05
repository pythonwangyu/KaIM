//
// Created by ka on 2021/1/13.
//

#include "KaTime.h"
#include "../Net/KaCallBack.h"

using namespace KaNet;

//静态成员的初始阿虎
std::atomic<int64_t> Timer::m_numberCreate;

//m_repeatCount如果等于0表明，这个计时器只执行一次

Timer::Timer(const TimerCallback &cb, Timestamp when, int64_t interval, int64_t repeatCount)
        : m_callback(cb), m_expiration(when), m_interval(interval),
          m_repeatCount(repeatCount), m_sequcence(++m_numberCreate), m_cancled(false) {

}

Timer::Timer(TimerCallback &&cb, Timestamp when, int64_t interval)
        : m_callback(std::move(cb)),
          m_expiration(when), m_interval(interval),
          m_repeatCount(-1), m_sequcence(++m_numberCreate), m_cancled(false) {}

//计时器对象执行函数
void Timer::Run() {
    if (m_cancled)       //如果计时器没有取消，那么执行回调函数
        return;
    m_callback();
    if (m_repeatCount != 0)         //如果m_repeatCount == -1，表明
    {
        --m_repeatCount;
        if (m_repeatCount == 0) {
            return;
        }
    }
    m_expiration += m_interval;     //改变时间戳。
}





LoopItem::LoopItem(const LoopFunction &cb, void *arg)
:m_loopfunc(cb),user_data(arg)
{}

void LoopItem::Run() {
    m_loopfunc(user_data);
}
