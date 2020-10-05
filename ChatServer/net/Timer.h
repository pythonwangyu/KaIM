#pragma once

#include <atomic>
#include <stdint.h>
#include "../base/Timestamp.h"
#include "../net/Callbacks.h"

namespace net
{ 
    ///
    /// Internal class for timer event.
    ///计时器事件
    /// 
    
    //计时器是什么被初始化的呢？

    class Timer
    {
    public:
        Timer(const TimerCallback& cb, Timestamp when, int64_t interval, int64_t repeatCount = -1);   



        Timer(TimerCallback&& cb, Timestamp when, int64_t interval);

        //运行计数器事件
        void run();
        

        //是否已取消
        bool isCanceled() const
        {
            return canceled_;
        }

        //取消函数
        void cancel(bool off)
        {
            canceled_ = off;
        }


        Timestamp expiration() const { return expiration_; }
        int64_t getRepeatCount() const { return repeatCount_; }
        int64_t sequence() const { return sequence_; }

 

        static int64_t numCreated() { return s_numCreated_; }

    private:
        //noncopyable，禁止拷贝
        Timer(const Timer& rhs) = delete;
        Timer& operator=(const Timer& rhs) = delete;

    private:
        //typedef std::function<void()> TimerCallback;              //定时器回调事件
        
        const TimerCallback         callback_;
        Timestamp                   expiration_;           //时间戳
        const int64_t               interval_;
        int64_t                     repeatCount_;       //重复次数，-1 表示一直重复下去
        const int64_t               sequence_;
        bool                        canceled_;          //是否处于取消状态

        static std::atomic<int64_t> s_numCreated_;
    };
}