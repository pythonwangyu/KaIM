#pragma once

#include <memory>
#include <functional>

#include "../base/Timestamp.h"

namespace net
{
	class EventLoop;

	///
	/// A selectable I/O channel.
	///
	/// This class doesn't own the file descriptor.
	/// The file descriptor could be a socket,
	/// an eventfd, a timerfd, or a signalfd
	class Channel
	{
	public:
		typedef std::function<void()> EventCallback;
		typedef std::function<void(Timestamp)> ReadEventCallback;
		//上面等价于使用c的传统函数指针写法
		//在函数名的前面要加上一个取值符
		/*typedef void(*ventCallback_c)();
		typedef void(*ReadEventCallbacj_c)(Timestamp);*/

		Channel(EventLoop* loop, int fd);
		~Channel();

		void handleEvent(Timestamp receiveTime);
		void setReadCallback(const ReadEventCallback& cb)
		{
			readCallback_ = cb;
		}
		void setWriteCallback(const EventCallback& cb)
		{
			writeCallback_ = cb;
		}
		void setCloseCallback(const EventCallback& cb)
		{
			closeCallback_ = cb;
		}
		void setErrorCallback(const EventCallback& cb)
		{
			errorCallback_ = cb;
		}

		/// Tie this channel to the owner object managed by shared_ptr,
		/// prevent the owner object being destroyed in handleEvent.
		void tie(const std::shared_ptr<void>&);

		int fd() const { return fd_; }
		int events() const { return events_; }
		void set_revents(int revt) { revents_ = revt; }  // used by pollers
        void add_revents(int revt) { revents_ |= revt; } // used by pollers
		// int revents() const { return revents_; }
		bool isNoneEvent() const { return events_ == kNoneEvent; }

        bool enableReading();
        bool disableReading();
        bool enableWriting();
        bool disableWriting();
        bool disableAll();

		bool isWriting() const { return events_ & kWriteEvent; }

		// for Poller
		int index() { return index_; }
		void set_index(int idx) { index_ = idx; }

		// for debug
		string reventsToString() const;

		void doNotLogHup() { logHup_ = false; }

		EventLoop* ownerLoop() { return loop_; }
		void remove();

	private:
		bool update();
		void handleEventWithGuard(Timestamp receiveTime);

		static const int            kNoneEvent;
		static const int            kReadEvent;
		static const int            kWriteEvent;

		EventLoop*                  loop_;		//循环指针
		const int                   fd_;		//文件描述符，或者说是套接字
		int                         events_;
		int                         revents_; // it's the received event types of epoll or poll
		int                         index_; // used by Poller.
		bool                        logHup_;

		std::weak_ptr<void>         tie_;           //std::shared_ptr<void>/std::shared_ptr<void>可以指向不同的数据类型
		bool                        tied_;

		//四种回调函数
		ReadEventCallback           readCallback_;
		EventCallback               writeCallback_;
		EventCallback               closeCallback_;
		EventCallback               errorCallback_;
	};
}