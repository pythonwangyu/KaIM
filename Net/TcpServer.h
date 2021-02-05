//
// Created by ka on 2021/1/15.
//

#ifndef KANET_TCPSERVER_H
#define KANET_TCPSERVER_H

#include <atomic>
#include <map>
#include <memory>
#include <functional>
#include "TcpConnection.h"

/*
 *  one thread one loop;
 *  while(true)
 *  {
 *      do_Loopthings();
 *      do_timequeue();
 *      fdnumber = epoll_wait(epfd,1);
 *      for(int i = 0; i < fdnumber; ++i)
 *      {
 *          handleWithEvent(fd);
 *      }
 * }
*/

namespace KaNet{

    class KaAcceptor;
    class EventLoop;
    class EventLoopThreadPool;

    class TcpServer{
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;

        explicit TcpServer(EventLoop* loop,KaNetAddr& listenAddr,std::string nameArg);
        ~TcpServer();

        const std::string& hostport() const{
            return m_hostPort;
        }
        //获取服务器的名字
        const std::string& name() const{
            return m_name;
        }
        //获取服务器的主循环
        EventLoop* GetLoop()const
        {
            return m_loop;
        }


        void start(int workThreadCount = 5);
        void stop();

        void setThreadInitCallback(const ThreadInitCallback& cb)
        {
            m_threadInitCallback = cb;
        }
        void setConnectionCallback(const ConnectionCallback& cb)
        {
            m_connectionCallback = cb;
        }

        void setMessageCallback(const MessageCallback& cb)
        {
            m_messageCallback = cb;
        }

        void setWriteCompleteCallback(const WriteCompleteCallback& cb)
        {
            m_writeCompleteCallback = cb;
        }

        void removeConnection(const TcpConnectionPtr& conn);

    private:
        //私有函数，创建新连接后的处理函数
        void newConnection(int sockfd,const KaNetAddr& peerAddr);

        void removeConnectionInLoop(const TcpConnectionPtr& conn);

        //存储TcpConnectionPtr的map，与string相关。
        typedef std::map<string,TcpConnectionPtr> ConnectionMap;

    private:
        EventLoop*                                          m_loop;                     //服务器所属的主循环
        const std::string                                   m_hostPort;
        const std::string                                   m_name;                     //服务器的名字
        std::unique_ptr<KaAcceptor>                         m_acceptor;                  //a acceptor wrap a channel for Listen
        std::unique_ptr<EventLoopThreadPool>                m_eventLoopThreadPool;      //wrap a ThreadPool
        ConnectionCallback                                  m_connectionCallback;
        MessageCallback                                     m_messageCallback;
        WriteCompleteCallback                               m_writeCompleteCallback;
        ThreadInitCallback                                  m_threadInitCallback;
        std::atomic<int>                                    m_started;
        int                                                 m_nextConnId;
        //核心的tcpConnection管理map<连接string,连接智能指针>
        ConnectionMap                                       m_connections;
    };

}//end of namespace KaNet;


#endif //KANET_TCPSERVER_H
