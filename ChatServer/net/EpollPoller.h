#pragma once

#ifndef WIN32

#include <vector>
#include <map>

#include "../base/Timestamp.h"
//#include "EventLoop.h"
#include "Poller.h"

struct epoll_event;

namespace net
{
    class EventLoop;

    class EPollPoller : public Poller
	{
	public:
		//构造函数需要传入loop指针。
		EPollPoller(EventLoop* loop);


		//虚析构函数，继承了Poller基类
		virtual ~EPollPoller();




		virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
		virtual bool updateChannel(Channel* channel);
		virtual void removeChannel(Channel* channel);

		virtual bool hasChannel(Channel* channel) const;

		//static EPollPoller* newDefaultPoller(EventLoop* loop);

        void assertInLoopThread() const;

	private:
		static const int kInitEventListSize = 16;


		//向激活channel链表中插入相应的channel指针
		void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
		bool update(int operation, Channel* channel);		

	private:
		typedef std::vector<struct epoll_event> EventList;

		int                                     epollfd_;		//create_epoll()返回的fd。
		EventList                               events_;		//接受返回的eventfd的结构体

		typedef std::map<int, Channel*>         ChannelMap;

		ChannelMap                              channels_;		//channel的映射表
		EventLoop*                              ownerLoop_;		//拥有者的循环，
	};

}

#endif