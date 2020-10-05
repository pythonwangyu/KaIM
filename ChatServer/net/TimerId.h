#pragma once

namespace net
{
    class Timer;

    ///
    /// An opaque identifier, for canceling Timer.
    ///
    class TimerId
    {
    public:
        TimerId(): timer_(NULL), sequence_(0)
        {
        }

        TimerId(Timer* timer, int64_t seq) : timer_(timer), sequence_(seq)
        {
        }

        Timer* getTimer()
        {
            return timer_;
        }

        // default copy-ctor, dtor and assignment are okay

        friend class TimerQueue;

    private:
        Timer*      timer_;     //指向计时器的指针
        int64_t     sequence_;         //序列号
    };

}
