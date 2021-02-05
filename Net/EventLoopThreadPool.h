//
// Created by ka on 2021/1/15.
//

#ifndef KANET_EVENTLOOPTHREADPOOL_H
#define KANET_EVENTLOOPTHREADPOOL_H

#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace KaNet{

    //前置声明
    class EventLoop;
    class EventLoopThread;

    //循环线程池
    class EventLoopThreadPool{
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;

        explicit EventLoopThreadPool();
        ~EventLoopThreadPool();

        void init(EventLoop* baseLoop,int numThreads);
        void start(const ThreadInitCallback& cb = ThreadInitCallback());
        void stop();

        EventLoop* getNextLoop();
        EventLoop* GetLoopForHash(size_t HashCode);
        std::vector<EventLoop*>  GetAllThreadLoop();
        bool statrted()  const
        {
            return  m_started;
        }

        std::string name() const{
            return m_name;
        }

        std::string info() const;

    private:
        EventLoop*                                      m_baseLoop;    //线程池所属的主循环
        std::string                                     m_name;
        bool                                            m_started;
        int                                             m_numberThreads;    //线程的数量
        int                                             m_next;
        std::vector<std::unique_ptr<EventLoopThread>>   m_threads;        //独占智能指针vector
        std::vector<EventLoop*>                         m_loops;         //循环指针vector
    };

    //普通线程池


}//end of namespace KaNet


#endif //KANET_EVENTLOOPTHREADPOOL_H
