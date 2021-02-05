//
// Created by ka on 2021/1/15.
//

#ifndef KANET_EVENTLOOPTHREAD_H
#define KANET_EVENTLOOPTHREAD_H


#include <condition_variable>
#include <thread>
#include <mutex>
#include <string>
#include <functional>
#include <memory>
#include <iostream>

namespace KaNet{

    class EventLoop;
    class EventLoopThread{
    public:

        typedef std::function<void(EventLoop*)>  ThreadInitCallback;

        explicit EventLoopThread(ThreadInitCallback  cb = ThreadInitCallback(),const std::string& name="");

        ~EventLoopThread();

        EventLoop* startLoop();

        void stopLoop();

    private:
        void threadFunc();                  //线程执行流函数

        EventLoop*                          m_loop;                 //loopthread，所属的循环
        bool                                m_exiting;
        std::unique_ptr<std::thread>        m_thread;               //loopthread所属的线程
        std::mutex                          m_mutex;                //互斥所
        std::condition_variable             m_cond;                 //条件变量
        ThreadInitCallback                  m_callback;
    };

}//end of namespace KaNet

#endif //KANET_EVENTLOOPTHREAD_H