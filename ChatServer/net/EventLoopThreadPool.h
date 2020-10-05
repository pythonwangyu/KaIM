#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <string>
//#include "EventLoop.h"


//其他的线程loop是如何被创建的？


namespace net
{

	class EventLoop;
	class EventLoopThread;

	class EventLoopThreadPool
	{
	public:
		typedef std::function<void(EventLoop*)> ThreadInitCallback;


		//构造函数，
		EventLoopThreadPool();


		~EventLoopThreadPool();
		
		//初始化基础的循环，设定循环线程的数量
		void init(EventLoop* baseLoop, int numThreads);



		//这个cb究竟是什么？
		void start(const ThreadInitCallback& cb = ThreadInitCallback());

        void stop();

		// valid after calling start()
		/// round-robin
		EventLoop* getNextLoop();

		/// with the same hash code, it will always return the same EventLoop
		EventLoop* getLoopForHash(size_t hashCode);

		std::vector<EventLoop*> getAllLoops();

		bool started() const
		{ return started_; }

		const std::string& name() const
		{ return name_; }

		const std::string info() const;

	private:

		EventLoop*                                      baseLoop_;
		std::string                                     name_;
		bool                                            started_;
		int                                             numThreads_;
		int                                             next_;
		std::vector<std::unique_ptr<EventLoopThread> >  threads_;
		std::vector<EventLoop*>                         loops_;	
	};

}