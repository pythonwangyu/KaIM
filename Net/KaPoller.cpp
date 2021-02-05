//
// Created by ka on 2021/1/13.
//

#include "KaPoller.h"
#include "../Util/Platfrom.h"
#include <cstring>

using namespace KaNet;


KaPoller::KaPoller(EventLoop *loop)
        : m_epollfd(::epoll_create(1)),
          m_events(InitEventList),
          m_ownerLoop(loop) {}

KaPoller::~KaPoller() {
    ::close(m_epollfd);
}




bool KaPoller::hasChannel(KaChannel *channel) {

    auto it = m_channels.find(channel->fd());
    return (it != m_channels.end()) && (it->second == channel);
}


Timestamp KaPoller::poll(int timeoutMs, ChannelList *activeChannels) {
    int numEvents = ::epoll_wait(m_epollfd,&*m_events.begin(),static_cast<int>(m_events.size()),
                                 timeoutMs);
    int Errno = errno;
    Timestamp now(Timestamp::now());        //获取当前时间戳
    if(numEvents > 0) {
        fillActiveChannel(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == m_events.size()) {
            m_events.resize(m_events.size() * 2);
        }
    }
    else if(numEvents == 0)
    {

    }
    else
    {
        if(Errno != EINTR)
        {
            errno = Errno;
        }
    }
    return now;
}


/*激活的fd添加到epoll_wait返回的Channel中去*/
void KaPoller::fillActiveChannel(int numEvents, ChannelList *activeChannels) {
    for(int i =0 ;i < numEvents; ++i)
    {
        KaChannel* channel = static_cast<KaChannel*>(m_events[i].data.ptr);
        int  fd = channel->fd();
        auto it = m_channels.find(fd);
        if(it == m_channels.end() || it->second != channel)
        {
            return;
        }
        channel->set_revent(m_events[i].events);        //将回调事件设置出
        activeChannels->push_back(channel);             //将梳理好后的channel放入到激活的Channel队列中
    }
}


//
bool KaPoller::updateChannel(KaChannel *channel) {
    const int index = channel->index();
    if(index == KNew || index == KDeleted)
    {
        int fd = channel->fd();
        if(index == KNew)
        {
            if(m_channels.find(fd) != m_channels.end())
            {
                return false;       //如果在Channelmap中发现了这个，这就表明index == KNew有错误
            }
            m_channels[fd] = channel;
        }
        else  //index == KDelete
        {
            if(m_channels.find(fd) == m_channels.end())
            {
                return false;
            }
            if(m_channels[fd] != channel)
            {
                return false;
            }
        }
        channel->setIndex(KAdded);
        return update(EPOLL_CTL_ADD,channel);
    }
    else            //KADDED == index
    {
        int fd = channel->fd();
        //判断错误
        if(m_channels.find(fd) == m_channels.end() || m_channels[fd] != channel || index != KAdded)
        {
            return false;
        }
        if(channel->isNonEvent())
        {
            if(update(EPOLL_CTL_DEL,channel))
            {
                channel->setIndex(KDeleted);
                return true;
            }
            return false;
        }
        else
        {
            return update(EPOLL_CTL_MOD,channel);
        }
    }
}



void KaPoller::removeChannel(KaChannel *channel) {
    int fd = channel->fd();
    if(m_channels.find(fd) == m_channels.end() || m_channels[fd] != channel || !channel->isNonEvent())
    {
        return;
    }
    int index = channel->index();
    if(index != KAdded && index != KDeleted)
    {
        return;
    }
    size_t n = m_channels.erase(fd);
    if(n != 1)
        return;
    if(index == KAdded)
    {
        update(EPOLL_CTL_DEL,channel);
    }
    channel->setIndex(KNew);
}


//实际调用epoll_ctl函数
bool KaPoller::update(int operation, KaChannel *channel) const {
    struct epoll_event event{};
    memset(&event,0,sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if(::epoll_ctl(m_epollfd,operation,fd,&event) < 0)
    {
        //syserror output
        return false;
    }
    return true;
}