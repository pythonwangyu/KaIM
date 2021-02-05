//
// Created by ka on 2021/1/15.
//

#ifndef KANET_KAACCEPTOR_H
#define KANET_KAACCEPTOR_H

#include "EventLoop.h"
#include "KaSocket.h"
#include "KaChannel.h"
#include "KaCallBack.h"

#include <functional>


namespace KaNet{
    class KaAcceptor{
    public:

        typedef std::function<void(int sockfd,const KaNetAddr&)> NewConnectionCallback;

        explicit KaAcceptor(EventLoop* loop,KaNetAddr& listenAddr);

        ~KaAcceptor();

        //设置新连接的函数
        void setNewConnectionCallback(const NewConnectionCallback& cb){
            m_newConnectionCallback = cb;
        }
        bool listening() const {
            return m_listening;
        }

        //调用listen函数之后才会将数据
        void listen();          //启动监听，enableReading()函数在内部调用，使得该套接字可以被监听。

    private:
        //private function
        void handleRead();      //设置为acceptChannel的读事件回调函数
    private:
        EventLoop*              m_loop;
        KaSocket                m_acceptSocket;
        KaChannel               m_acceptChannel;
        NewConnectionCallback   m_newConnectionCallback;            //新的连接建立的时候调用的函数，由handleRead函数调用
        bool                    m_listening;
    };
}


#endif //KANET_KAACCEPTOR_H
