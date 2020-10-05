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
		//���캯����Ҫ����loopָ�롣
		EPollPoller(EventLoop* loop);


		//�������������̳���Poller����
		virtual ~EPollPoller();




		virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
		virtual bool updateChannel(Channel* channel);
		virtual void removeChannel(Channel* channel);

		virtual bool hasChannel(Channel* channel) const;

		//static EPollPoller* newDefaultPoller(EventLoop* loop);

        void assertInLoopThread() const;

	private:
		static const int kInitEventListSize = 16;


		//�򼤻�channel�����в�����Ӧ��channelָ��
		void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
		bool update(int operation, Channel* channel);		

	private:
		typedef std::vector<struct epoll_event> EventList;

		int                                     epollfd_;		//create_epoll()���ص�fd��
		EventList                               events_;		//���ܷ��ص�eventfd�Ľṹ��

		typedef std::map<int, Channel*>         ChannelMap;

		ChannelMap                              channels_;		//channel��ӳ���
		EventLoop*                              ownerLoop_;		//ӵ���ߵ�ѭ����
	};

}

#endif