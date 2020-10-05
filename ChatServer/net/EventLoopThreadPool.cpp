#include "EventLoopThreadPool.h"
#include <stdio.h>
#include <assert.h>
#include <sstream>
#include <string>
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "Callbacks.h"

using namespace net;


EventLoopThreadPool::EventLoopThreadPool()
    : baseLoop_(NULL),
    started_(false),
    numThreads_(0),
    next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    // Don't delete loop, it's stack variable
}

void EventLoopThreadPool::init(EventLoop* baseLoop, int numThreads)
{
    numThreads_ = numThreads;
    baseLoop_ = baseLoop;
}


//传递工作函数进入循环事件池
void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    //assert(baseLoop_);
    if (baseLoop_ == NULL)
        return;
       
    //这时候started_还没有创建
    //assert(!started_);
    if (started_)
        return;

    baseLoop_->assertInLoopThread();

    started_ = true;

    for (int i = 0; i < numThreads_; ++i)
    {
        char buf[128];
        snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);

        //创建出循环线程对象，这个对象之中含有线程对象。
        std::unique_ptr<EventLoopThread> t(new EventLoopThread(cb, buf));


        //EventLoopThread* t = new EventLoopThread(cb, buf);	


        loops_.push_back(t->startLoop());
        threads_.push_back(std::move(t));   //使用了移动语句
    }
    if (numThreads_ == 0 && cb)
    {
        cb(baseLoop_);
    }
}

void EventLoopThreadPool::stop()
{
    for (auto& iter : threads_)
    {
        iter->stopLoop();
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    //assert(started_);
    if (!started_)
        return NULL;

    EventLoop* loop = baseLoop_;

    if (!loops_.empty())
    {
        // round-robin              //轮询机制
        loop = loops_[next_];       //刚开始的时候，next_为0，在EventLoopThreadPool中初始化。
        ++next_;
        if (implicit_cast<size_t>(next_) >= loops_.size())  //注意这里的implicit_cast
        {
            next_ = 0;
        }
    }
    return loop;            //返回循环之中的数据。
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
    baseLoop_->assertInLoopThread();
    EventLoop* loop = baseLoop_;

    if (!loops_.empty())
    {
        loop = loops_[hashCode % loops_.size()];
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    baseLoop_->assertInLoopThread();
    if (loops_.empty())
    {
        return std::vector<EventLoop*>(1, baseLoop_);
    }
    else
    {
        return loops_;
    }
}

const std::string EventLoopThreadPool::info() const
{
    std::stringstream ss;
    ss << "print threads id info " << endl;
    for (size_t i = 0; i < loops_.size(); i++)
    {
        ss << i << ": id = " << loops_[i]->getThreadID() << endl;
    }
    return ss.str();
}