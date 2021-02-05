//
// Created by ka on 2021/1/13.
//

#ifndef KANET_KACHANNEL_H
#define KANET_KACHANNEL_H

#include "../time/TimeStamp.h"
#include <functional>
#include <memory>


/*
 *          const int KNew = -1;            //初始化创建
            const int KAdded = 1;
            const int KDeleted = 2;
 *
 * */

namespace KaNet{

    //前置声明
    class EventLoop;

    class KaChannel{
    public:
        //内嵌声明
        typedef std::function<void()> EventCallBack;
        typedef std::function<void(Timestamp)> ReadEventCallBack;

        explicit KaChannel(EventLoop* loop,int fd);
        ~KaChannel();

        void handleEvent(Timestamp receiveTime);

        void setReadCallback(const ReadEventCallBack& cb){
            m_readCallback = cb;
        }
        void setWriteCallback(const EventCallBack& cb){
            m_writeCallback = cb;
        }

        void setCloseCallback(const EventCallBack& cb){
            m_closeCallback = cb;
        }

        void setErrorCallback(const EventCallBack& cb){
            m_errorCallback = cb;
        }

        void tie(const std::shared_ptr<void>&);

        int fd() const{return m_fd;}

        int events() const{return m_events;}

        void set_revent(int revent){
            m_revents = revent;
        }
        void add_revent(int revent){
            m_revents |= revent;
        }

        bool isNonEvent() const { return m_events == NoneEvent; }

        bool enableReading();
        bool disableReading();
        bool enableWriting();
        bool disableWriting();
        bool disableAll();

        bool isWriting() const{
            return m_events & WriteEvent;
        }

        int index() const{
            return m_index;
        }
        void setIndex(int index){
            m_index = index;
        }

        EventLoop * GetLoop() {
            return m_loop;
        }

        void remove();
    private:
        //private function
        bool update();
        void handleEventWithGuard(Timestamp receiveTime);

        static const int        NoneEvent;
        static const int        ReadEvent;
        static const int        WriteEvent;


    private:
        EventLoop*              m_loop;
        const int               m_fd;
        int                     m_events;
        int                     m_revents;
        int                     m_index;
        bool                    m_logHup;

        std::weak_ptr<void>     m_tie;              //这个是与TcpConnection相关联的弱指针
        bool                    m_tied;

        ReadEventCallBack       m_readCallback;
        EventCallBack           m_writeCallback;
        EventCallBack           m_closeCallback;
        EventCallBack           m_errorCallback;

    };

}//end of the namespace KaNet


#endif //KANET_KACHANNEL_H
