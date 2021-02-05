//
// Created by ka on 2021/2/5.
//

#ifndef TEST_CHATSESSION_H
#define TEST_CHATSESSION_H

#include "../Net/TcpSessin.h"
#include "../time/TimeStamp.h"
#include "../Net/Buffer.h"

#include <string>
#include <map>
#include <functional>

typedef int32_t CMD;

using namespace KaNet;


typedef std::function<void(void*)>  cmdFun;

//CMD对应的函数
static std::map<CMD,cmdFun> cmdMap;


//用户信息
struct UserInfo{
    int32_t             userId;
    std::string         userName;
  //  std::string         nickName;
    std::string         password;
    int32_t             status;
};







class ChatSession : public TcpSession {
public:
    explicit ChatSession(const std::shared_ptr<TcpConnection> &conn, int sessionId);

    virtual ~ChatSession();
    //no copy
    ChatSession(const ChatSession &obj) = delete;
    ChatSession &operator=(const ChatSession &obj) = delete;

    void OnRead(const std::shared_ptr<TcpConnection>& conn,Buffer* pBuffer,Timestamp receivRime);

    int32_t getSessionId(){
        return m_id;
    }
    int32_t getUserId(){
        return m_userInfo.userId;
    }
    std::string getUsername(){
        return m_userInfo.userName;
    }
    std::string getPassword(){
        return m_userInfo.password;
    }
    std::string getUserStatus(){
        return m_userInfo.status;
    }

private:
    bool process(const std::shared_ptr<TcpConnection>& conn,const char* buf,size_t buflength);




private:
    int32_t                 m_id;                       //session Id
    UserInfo                m_userInfo;                 //用户信息
    int32_t                 m_seq;                      //当前包序列
    bool                    m_isLogin;                  //当前的session是否登录的标志
    time_t                  m_lastPackageTime;          //上一次收发包的数量
};


#endif //TEST_CHATSESSION_H
