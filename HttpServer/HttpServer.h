//
// Created by ka on 2021/1/18.
//

#ifndef KANET_HTTPSERVER_H
#define KANET_HTTPSERVER_H


#include "../TcpServer.h"
#include "../EventLoop.h"
#include <memory>
#include <list>
#include <mutex>


namespace KaNet{

    class HttpSession;

    class HttpServer    final {
    public:
        HttpServer() = default;
        ~HttpServer() = default;
        HttpServer(const HttpServer& rhs)=delete;
        HttpServer& operator=(const HttpServer& rhs) = delete;

    public:
        bool init(const char* ip,short port,EventLoop* loop);
        void uninit();

        void onConnected(std::shared_ptr<TcpConnection> conn);

        void onDisconnected(const std::shared_ptr<TcpConnection>& Conn);

    private:
        std::unique_ptr<TcpServer>                      m_server;
        std::list<std::shared_ptr<HttpSession>>         m_sessions;
        std::mutex                                      m_sessionMutex;
    };
}//end of namespace KaNet


#endif //KANET_HTTPSERVER_H
