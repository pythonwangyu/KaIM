//
// Created by ka on 2021/1/13.
//

#ifndef KANET_EVENTLOOP_H
#define KANET_EVENTLOOP_H

#include <functional>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <atomic>

#include "../Util/Platfrom.h"
#include "../time/TimeStamp.h"
#include "../time/KaTime.h"
#include "../time/TimerQueue.h"
#include "KaPoller.h"


namespace KaNet{

    //前置声明
    class EventLoop;
    class KaChannel;
    class Poller;
    class TimerQueue;

    typedef std::unique_ptr<TimerQueue> TimerQueuePtr;

//循环结构体，
    class EventLoop{
    public:
        typedef std::function<void()> Functor;
        explicit EventLoop();
        ~EventLoop();

        void loop();

        void quit();

        Timestamp pollReturnTime() const{
            return m_pollReturnTime;
        }

        int64_t iteration()const{return m_interation;}



//        void RunInLoop(const Functor& cb);
//
//        void queueInLoop(const Functor& cb);




        bool updateChannel(KaChannel* channel);
        void removeChannel(KaChannel* channel);
        bool hasChannel(KaChannel* channel);


        bool isInLoopThread()const{
            return m_threadId == std::this_thread::get_id();
        }

        bool eventhanding() const{
            return m_eventHanding;
        }

        const std::thread::id getThreadID() const{
            return m_threadId;
        }

    private:
        //private function
        bool createWakeUpFd();
        bool wakeUp() const;
        bool handleRead() const;
        void doPendingFunctors();



    private:
        typedef std::vector<KaChannel*> ChannelList;
        //原子量布尔值，保证线程安全
        std::atomic_bool                        m_looping;
        std::atomic_bool                        m_quit;
        std::atomic_bool                        m_eventHanding;
        std::atomic_bool                        m_callingPendingFunctors;   //是否执行额外函数
        const std::thread::id                   m_threadId;                 //当前线程的Loop
        Timestamp                               m_pollReturnTime;           //返回的时间戳
        //Poll函数返回的事件戳，表示进行epoll_wait函数时候的事件戳。

        std::unique_ptr<Poller>                 m_Poller;                   //IO复用器。
        std::unique_ptr<TimerQueue>             m_TimerQueue;               //计时器队列
        int64_t                                 m_interation;
        socket_t                                m_wakeupFd{};               //唤醒套接字
        std::unique_ptr<KaChannel>              m_wakeUpChannel;            //唤醒套接字Channel
        ChannelList                             m_activeChannels;           //存储激活的Channel
        KaChannel*                              m_currentActiveChannel;     //当前激活的Channel
        std::mutex                              m_mutex;                    //互斥锁
        std::vector<Functor>                    m_pendingFunctors;          //额外函数集
    };

}//end of namespace KaNet;

#endif //KANET_EVENTLOOP_H
