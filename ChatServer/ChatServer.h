//
// Created by ka on 2021/1/22.
//

#ifndef KANET_CHATSERVER_H
#define KANET_CHATSERVER_H

#include "../Net/EventLoop.h"

#include <memory>
#include <list>
#include <map>


using namespace KaNet;



class ChatServer final {
public:
    explicit ChatServer();
    ~ChatServer() = default;

    //no copy
    ChatServer(const ChatServer& rhs) = delete;
    ChatServer& operator=(const ChatServer& obj) = delete;

    bool init(const char* ip,short port,EventLoop* loop);
    bool unint();

    void getSessions(std::list<std::shared_ptr<>>)


};


#endif //KANET_CHATSERVER_H
