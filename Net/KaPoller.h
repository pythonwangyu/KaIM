//
// Created by ka on 2021/1/13.
//

#ifndef KANET_KAPOLLER_H
#define KANET_KAPOLLER_H

#include <vector>
#include "../time/TimeStamp.h"
#include "EventLoop.h"
#include <map>

namespace KaNet{

    //index的变化
    const int KNew = -1;
    const int KAdded = 1;
    const int KDeleted = 2;

    //前置声明
    class KaChannel;
    class EventLoop;

    /*抽象类无法被实例化只能够继承*/
    class Poller{
    public:
        Poller()=default;
        ~Poller() = default;
        typedef std::vector<KaChannel*> ChannelList;
        virtual Timestamp poll(int timeoutMs,ChannelList* activeChannels) = 0;
        virtual bool updateChannel(KaChannel* channel) = 0;
        virtual void removeChannel(KaChannel* channel) = 0;
        virtual bool hasChannel(KaChannel* channel) = 0;
    private:
    };


    class KaPoller: public Poller
    {
    public:
        KaPoller(EventLoop *loop);
        virtual ~KaPoller();        //虚析构函数，用于释放资源

        virtual Timestamp poll(int timeoutMs,ChannelList* activeChannels)final;

        virtual bool updateChannel(KaChannel* channel) final;

        virtual void removeChannel(KaChannel* channel) final;

        virtual bool hasChannel(KaChannel* channel) final;

//        void assertInLoopThread() const;
    private:
        //private function

        static const int InitEventList = 16;
        void fillActiveChannel(int numEvents,ChannelList* activeChannels);
        bool update(int operation,KaChannel* channel) const;


    private:
        //类内嵌声明
        typedef std::vector<struct epoll_event> EventList;
        typedef std::map<int,KaChannel*>        ChannelMap;
        int                         m_epollfd;
        EventList                   m_events;
        ChannelMap                  m_channels;
        EventLoop*                  m_ownerLoop;

    };

}//end of namespace KaNet


#endif //KANET_KAPOLLER_H
