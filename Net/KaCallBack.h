//
// Created by ka on 2021/1/13.
//

#ifndef KANET_KACALLBACK_H
#define KANET_KACALLBACK_H

#include <memory>
#include <functional>
namespace KaNet{
    class Buffer;
    class TcpConnection;

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void()> TimerCallback;
    typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
    typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;

    typedef std::function<void(const TcpConnectionPtr&,size_t)> HighWaterMarkCallback;

    typedef std::function<void(const TcpConnectionPtr& ,Buffer*,Timestamp)> MessageCallback;

    void defaultConnectionCallback(const TcpConnectionPtr& conn);

    void defaultMessageCallback(const TcpConnectionPtr& conn,Buffer* buff,Timestamp receiveTime);


}


#endif //KANET_KACALLBACK_H
