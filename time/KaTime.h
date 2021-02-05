//
// Created by ka on 2021/1/13.
//

#ifndef KANET_KATIME_H
#define KANET_KATIME_H

#include "TimeStamp.h"
#include "../Net/KaCallBack.h"
#include <atomic>
#include <cstdint>


/*
 *    计时器在指定时间戳内运行一次，m_repeatCount = 1；表示在指定时间戳内运行一次。
 *    计时器在指定时间戳内运行n次数，m_repeatCount = n;    但是在时间戳到期的时候如果没有运行指定次数也会使得时间
 *    戳失效。
 *
 *
 * */


namespace KaNet {
    //计时器类
    class Timer {
    public:
        //默认不重复
        Timer(const TimerCallback &cb, Timestamp when, int64_t interval, int64_t repeatCount = 0);

        Timer(TimerCallback &&cb, Timestamp when, int64_t interval);

        void Run();

        bool isCancled() const { return m_cancled; }

        void Control(bool off) {
            m_cancled = off;
        }

        Timestamp exprivation() {
            return m_expiration;
        }

        int64_t GetRepeatCount() const {
            return m_repeatCount;
        }

        int64_t sequence() const {
            return m_sequcence;
        }

        static int64_t GetNumberCreated() {
            return m_numberCreate;
        }

    private:
        const TimerCallback m_callback;                 //回调函数
        Timestamp m_expiration;                         //时间戳
        const int64_t m_interval;                       //时间间隔
        int64_t m_repeatCount;                          //计时器重复的次数
        const int64_t m_sequcence;                      //序列号
        bool m_cancled;                                 //是否取消的标志
        static std::atomic<int64_t> m_numberCreate;     //创建的计时器的个数
    };


    //持续循环器,只要这个循环器对象没有从队列中移除，就会一直运行
    typedef std::function<void(void*)>  LoopFunction;
    class LoopItem{
    public:
        LoopItem(const LoopFunction& cb,void* arg);
        void Run();
        ~LoopItem() = default;
    private:
        LoopFunction        m_loopfunc;
        void*               user_data;
    };

}//end of namespace of KaNet;
#endif //KANET_KATIME_H
