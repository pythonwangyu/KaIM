#pragma once

#include <mutex>
#include <condition_variable> 
#include <thread>
#include <string>
#include <functional>
//#include "EventLoop.h"

namespace net
{

	class EventLoop;

	class EventLoopThread
	{
	public:
		typedef std::function<void(EventLoop*)> ThreadInitCallback;

		EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(), const std::string& name = "");
		~EventLoopThread();
		EventLoop* startLoop();
        void stopLoop();

	private:
		void threadFunc();

		EventLoop*                   loop_;		//ѭ���¼�
		bool                         exiting_;		
		std::unique_ptr<std::thread> thread_;	//��ռʽ������ָ�뺯��
		std::mutex                   mutex_;
		std::condition_variable      cond_;
		ThreadInitCallback           callback_;	//�߳�ִ�к���
	};

}