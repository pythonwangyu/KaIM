//
// Created by ka on 2021/1/18.
//

#ifndef KANET_HTTPSESSION_H
#define KANET_HTTPSESSION_H

#include "../Buffer.h"
#include "../TimeStamp.h"
#include "../TcpConnection.h"


namespace KaNet{

    class HttpSession{
    public:
        HttpSession(std::shared_ptr<TcpConnection>& conn);
        ~HttpSession() = default;


    private:

    };
}

#endif //KANET_HTTPSESSION_H
