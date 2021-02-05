//
// Created by ka on 2021/1/15.
//

#include "TcpConnection.h"
#include "KaChannel.h"
#include "EventLoop.h"
#include "KaSocket.h"


#include <thread>
#include <cerrno>
#include <functional>
#include <utility>




using namespace KaNet;

void KaNet::defaultConnectionCallback(const TcpConnectionPtr &conn) {
    ///
}

void KaNet::defaultMessageCallback(const TcpConnectionPtr& conn,Buffer* buff,Timestamp receiveTime){
    ///
}


TcpConnection::TcpConnection(EventLoop *loop, std::string name, int sockfd, const KaNetAddr &localAddr,
                             const KaNetAddr &peerAddr)
        : m_loop(loop), m_name(std::move(name)), m_state(Connecting),
          m_socket(new KaSocket(sockfd)),
          m_channel(new KaChannel(loop, sockfd)),
          m_localAddr(localAddr), m_peerAddr(peerAddr),
          m_highWaterMark(64 * 1024 * 1024) {
    m_channel->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    m_channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    m_channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    m_channel->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    m_socket->setKeepAlive(true);
}

TcpConnection::~TcpConnection() = default;

void TcpConnection::send(const void *data, int len) {
    if (m_state == Connected) {
        int32_t nwrote = 0;
        size_t remaining = len;
        if (DisConnected == m_state) {
            return;
        }
        if(!m_channel->isWriting() && m_outputBuffer.readableBytes() == 0)
        {
            nwrote = ::write(m_channel->fd(),data,len);
            if(nwrote >= 0)
            {
                remaining = len - nwrote;
                if(remaining == 0 && m_writeConpleteCallback)
                {
                    m_writeConpleteCallback(shared_from_this());
                }
            }
            else
            {
                nwrote = 0;
                if(errno != EWOULDBLOCK)
                {
                    return;
                }
            }
        }
        if(remaining > len)     //？
        {
            return;
        }
    }
}


void TcpConnection::send(const std::string &message) {

}

void TcpConnection::send(Buffer *message) {

}


void TcpConnection::shutdown() {
    if(m_state == Connected)
    {
        setState(Disconnecting);
        if(m_channel->isWriting())
        {
            m_socket->shutdownWrite();
        }
    }
}


void TcpConnection::forceClose() {
    if(m_state == Connected || m_state == Disconnecting)
    {
        setState(Disconnecting);
        handleClose();
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    m_socket->setNoDelay(true);
}

void TcpConnection::connectEatablished() {
    if(m_state != Connecting)
    {
        return;
    }
    setState(Connected);
    m_channel->tie(shared_from_this());
    if(!m_channel->enableReading())
    {
        handleClose();
        return;
    }
    m_connectionCallback(shared_from_this());       //这个函数由TcpServer的函数设置
}

void TcpConnection::connectDestroyed() {

}


/***************************************************private function********************************************/

//是否有必要检查线程，是否会存在线程不安全的情况？

void TcpConnection::handleRead(Timestamp receiveTime) {
    int saveError = 0;
    //将fd中的数据读取到接受缓冲区中
    int32_t n = m_inputBuffer.readFd(m_channel->fd(), &saveError);
    if (n > 0)
    {
        //调用m_messageCallback函数，这个函数是什么？被设置为TcpServer::m_messageCallback函数
        //首先是defaultMessageCallbackl这个在TcpServer初始化的时候被设置，
        //后面被设置为
        m_messageCallback(shared_from_this(),&m_inputBuffer,receiveTime);
    }
    else if(n == 0)
    {
        handleClose();
    }
    else
    {
        errno = saveError;
        handleError();
    }
}

//处理写事件
/*
 * 如果输入缓冲区存在内容，并且Channel可写，将输入缓冲区中的数据写入channel->fd()中去。
 * */
void TcpConnection::handleWrite() {
    if(m_channel->isWriting())      //首先是该Channel可写
    {
        //实际发送函数
        int32_t n = ::write(m_channel->fd(),m_outputBuffer.peek(),m_outputBuffer.readableBytes());
        if(n > 0)
        {
            m_outputBuffer.retrieve(n);
            if(m_outputBuffer.readableBytes() == 0)
            {
                m_channel->disableWriting();
                if(m_writeConpleteCallback)
                {
                    m_writeConpleteCallback(shared_from_this());
                }
                if(m_state == DisConnected)
                {
                    shutdown();
                }
            }
        }
        else    //处理关闭事件
        {
            handleClose();
        }
    }
}


void TcpConnection::handleClose() {
    if(m_state == DisConnected)
    {
        return;
    }
    setState(DisConnected);
    m_channel->disableAll();
    TcpConnectionPtr guardThis(shared_from_this());
    m_connectionCallback(guardThis);
    m_CloseCallback(guardThis);
}


void TcpConnection::handleError() {
    handleClose();
}





const char * TcpConnection::GetSateToString() const {
    switch (m_state) {
        case DisConnected:
            return "DisConnected";
        case Connecting:
            return "Connecting";
        case Connected:
            return "Connected";
        case Disconnecting:
            return "Disconnection";
        default:
            return "unknow state";
    }
}