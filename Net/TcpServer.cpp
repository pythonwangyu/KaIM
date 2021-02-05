//
// Created by ka on 2021/1/15.
//

#include "TcpServer.h"
#include "KaAcceptor.h"
#include "EventLoopThreadPool.h"
#include "KaSocket.h"

#include <cstdio>
#include <memory>


using namespace KaNet;
TcpServer::TcpServer(EventLoop *loop, KaNetAddr &listenAddr, std::string nameArg)
        : m_loop(loop), m_hostPort(listenAddr.ToString()), m_name(nameArg),
          m_acceptor(new KaAcceptor(loop, listenAddr)),
          m_connectionCallback(defaultConnectionCallback),
          m_messageCallback(defaultMessageCallback),
          m_started(0), m_nextConnId(1) {
    m_acceptor->setNewConnectionCallback(
            std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}


TcpServer::~TcpServer() {
    stop();
}

void TcpServer::start(int workThreadCount) {
    if(m_started == 0)
    {
        m_eventLoopThreadPool = std::make_unique<EventLoopThreadPool>();
        m_eventLoopThreadPool->init(m_loop,workThreadCount);
        m_eventLoopThreadPool->start();
        m_acceptor->listen();
        //m_loop->RunInLoop(std::bind(&KaAcceptor::listen,m_acceptor.get()));
        m_started = 1;
    }
}

void TcpServer::stop() {
    if(m_started == 0)
    {
        return;
    }
    for(auto & m_connection : m_connections)
    {
        TcpConnectionPtr  conn = m_connection.second;
        m_connection.second.reset();
        conn->connectDestroyed();
        //        conn->GetLoop()->RunInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
        conn.reset();
    }
    m_eventLoopThreadPool->stop();
    m_started = 0;
}


void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    removeConnectionInLoop(conn);
   // m_loop->RunInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,conn));
}


/**Private function***/
//核心函数，就是这个
//新的连接连接上之后的情况，会创建TcpCinnection由智能指针托管。
void TcpServer::newConnection(int sockfd, const KaNetAddr &peerAddr) {
    EventLoop *ioLoop = m_eventLoopThreadPool->getNextLoop();
    char buf[32] = {0};
    snprintf(buf, sizeof buf, ":%s#%d", m_hostPort.c_str(), m_nextConnId);
    ++m_nextConnId;
    std::string connName = m_name + buf;
    KaNetAddr localAddr(KaNet::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
    //这里的conn的引用计数为1，
    m_connections[connName] = conn;
    //这里的引用计数为2
    conn->setConnectionCallback(m_connectionCallback);
    conn->setMessageCallback(m_messageCallback);
    conn->setWriteCompleteCallback(m_writeCompleteCallback);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    conn->connectEatablished();
    //ioLoop->RunInLoop(std::bind(&TcpConnection::connectEatablished,conn));
}

//将制定的TcpConntionPtr移除
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    size_t n = m_connections.erase(conn->name());
    if(n != 1)
    {
        return;         //创建TCP连接的时候对方断开了连接
    }
    EventLoop* ioLoop = conn->GetLoop();
    conn->connectDestroyed();
   // ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
}


//为什么要在Loop中移除？