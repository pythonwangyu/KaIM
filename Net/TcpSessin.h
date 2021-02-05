//
// Created by ka on 2021/1/18.
//

#ifndef KANET_TCPSESSIN_H
#define KANET_TCPSESSIN_H

#include <memory>
#include "TcpConnection.h"


namespace KaNet{

    class TcpSession{
    public:
        explicit TcpSession(const std::weak_ptr<TcpConnection>& tempconn);
        ~TcpSession() = default;
        TcpSession(const TcpSession& conn) = delete;
        TcpSession& operator=(const TcpSession& conn) = delete;

        std::shared_ptr<TcpConnection> GetConnectionPtr()
        {
            if(m_tmpConn.expired())
            {
                return nullptr;
            }
            return m_tmpConn.lock();
        }

    private:
        void sendPack(const char* p,int32_t length);

    protected:
        std::weak_ptr<TcpConnection>            m_tmpConn;          //弱指针
    };
}


#endif //KANET_TCPSESSIN_H
