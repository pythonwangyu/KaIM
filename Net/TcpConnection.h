//
// Created by ka on 2021/1/15.
//

#ifndef KANET_TCPCONNECTION_H
#define KANET_TCPCONNECTION_H

#include <memory>
#include "Buffer.h"
#include "KaSocket.h"
#include "KaCallBack.h"
#include "../time/TimeStamp.h"

#include <string>

namespace KaNet {

    //前置声明
    class EventLoop;

    class KaChannel;

    class KaSocket;


    class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
    public:
        TcpConnection(EventLoop *loop, std::string name, int sockfd, const KaNetAddr &localAddr,
                      const KaNetAddr &peerAddr);

        ~TcpConnection();


        EventLoop *GetLoop() const { return m_loop; }

        const std::string &name() const { return m_name; }

        const KaNetAddr &localAddr() const { return m_localAddr; }

        const KaNetAddr &peerAddr() const { return m_peerAddr; }

        bool connected() const { return m_state == Connected; }

        void send(const void *message, int len);

        void send(const std::string &message);

        void send(Buffer *message);

        void shutdown();

        void forceClose();

        void setTcpNoDelay(bool on);

        void setConnectionCallback(const CloseCallback &cb) {
            m_connectionCallback = cb;
        }

        void setMessageCallback(const MessageCallback &cb) {
            m_messageCallback = cb;
        }

        void setWriteCompleteCallback(const WriteCompleteCallback &cb) {
            m_writeConpleteCallback = cb;
        }

        void setCloseCallback(const CloseCallback &cb) {
            m_CloseCallback = cb;
        }

        void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark) {
            m_highWaterMarkCallback = cb;
            m_highWaterMark = highWaterMark;
        }


        //连接后的行为
        void connectEatablished();

        //摧毁连接的行为
        void connectDestroyed();

        Buffer *inputBuffer() {
            return &m_inputBuffer;
        }

        Buffer *outputBuffer() {
            return &m_outputBuffer;
        }

    private:
        //private function
        enum StateE {            //TcpConnection的状态
            DisConnected,       //未连接
            Connecting,         //连接中
            Connected,          //已经连接了
            Disconnecting       //关闭连接中
        };
        //核心处理函数，Channel的读函数
        void handleRead(Timestamp receiveTime);
        //写函数
        void handleWrite();
        //关闭函数
        void handleClose();
        //处理error函数
        void handleError();
        //设置TcpConnection类的状态
        void setState(StateE s) {
            m_state = s;
        }

        const char *GetSateToString() const;        //for Debug;
    private:
        EventLoop                           *m_loop;       //该连接属于哪一个Loop
        const std::string                   m_name;       //该连接的名字
        std::atomic<StateE>                 m_state;                        //连接的状态
        std::unique_ptr<KaSocket>           m_socket;       //一个连接有一个KaSocket
        std::unique_ptr<KaChannel>          m_channel;      //一个连接有一个KaChannel
        const KaNetAddr                     m_localAddr;    //连接的本地地址
        const KaNetAddr                     m_peerAddr;     //连接的对端地址
        //回调函数
        ConnectionCallback                  m_connectionCallback;
        MessageCallback                     m_messageCallback;
        WriteCompleteCallback               m_writeConpleteCallback;
        CloseCallback                       m_CloseCallback;
        HighWaterMarkCallback               m_highWaterMarkCallback;
        size_t                              m_highWaterMark;
        //接受缓冲区，发送缓冲区
        Buffer                              m_inputBuffer;              //接受缓冲区，用于接受数据
        Buffer                              m_outputBuffer;             //发送缓冲区，用于发送数据
    };
}//end of namespace KaNet;
#endif //KANET_TCPCONNECTION_H