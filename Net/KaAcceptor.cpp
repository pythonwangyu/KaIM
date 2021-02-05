//
// Created by ka on 2021/1/15.
//

#include "KaAcceptor.h"

using namespace KaNet;


KaAcceptor::KaAcceptor(EventLoop *loop, KaNetAddr &listenAddr)
        : m_loop(loop),
          m_acceptSocket(KaSocket()),
          m_acceptChannel(loop, m_acceptSocket.fd()),
          m_listening(false) {
    m_acceptSocket.setReusePort(true);
    m_acceptSocket.setReuseAddr(true);
    m_acceptSocket.bindAddr(listenAddr);
    //设置handleRead函数为接收器的handleRead函数。
    m_acceptChannel.setReadCallback(std::bind(&KaAcceptor::handleRead, this));
}

//析构的时候移除KaChannel
KaAcceptor::~KaAcceptor() {
    m_acceptChannel.disableAll();
    m_acceptChannel.remove();
}

//启动监听实际底层调用的是enbaleReading()函数
void KaAcceptor::listen() {
    m_listening = true;
    m_acceptSocket.listen(SOMAXCONN);
    m_acceptChannel.enableReading();
}

void KaAcceptor::handleRead() {
    KaNetAddr peeraddr;
    socklen_t  len = sizeof peeraddr.GetAddr_in();
    //调用::accept函数，返回连接的客户端
    int connfd = m_acceptSocket.accept(peeraddr,&len);
    if(connfd >= 0)
    {
        std::string hostPort = peeraddr.ToString();
        if(m_newConnectionCallback) {
            //传入connfd就是新的套接字
            //TcpServer::newConnection()初始化TcpServer的时候设置newConnectionCallback
            m_newConnectionCallback(connfd, peeraddr);
        }
//            void TcpServer::newConnection(int sockfd, const KaNetAddr &peerAddr) {
//                EventLoop* ioLoop = m_eventLoopThreadPool->getNextLoop();
//                char buf[32] = {0};
//                snprintf(buf,sizeof buf,":%s#%d",m_hostPort.c_str(),m_nextConnId);
//                ++m_nextConnId;
//                std::string connName = m_name + buf;
//                KaNetAddr   localAddr(KaNet::getLocalAddr(sockfd));
//                TcpConnectionPtr conn(new TcpConnection(ioLoop,connName,sockfd,localAddr,peerAddr));
//                //这里的conn的引用计数为1，
//                m_connections[connName] = conn;
//                //这里的引用计数为2
//                conn->setConnectionCallback(m_connectionCallback);
//                conn->setMessageCallback(m_messageCallback);
//                conn->setWriteCompleteCallback(m_writeCompleteCallback);
//                conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,std::placeholders::_1));
//                conn->connectEatablished();
//                //ioLoop->RunInLoop(std::bind(&TcpConnection::connectEatablished,conn));
//            }
        else
        {
            KaNet::close(connfd);
        }
    }
    else
    {
        //DEBUG函数退出
    }
}
