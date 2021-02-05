//
// Created by ka on 2021/2/5.
//

#include "ChatSession.h"
#include "../MsgDefine/MsgDefine.h"
#include "../Util/KaZlibUtil.h"

#define MAX_PACKAGESIZE 10*1024*1024


/*
 *          i want to stay
 *
 *          MainCMD决定使用哪一个protobuf的读写器
 *          std::pair<READ,WRITE>       READERWRITER
 *          std::map<CMD,READERWRITER>
 *
 *
 * */


ChatSession::ChatSession(const std::shared_ptr<TcpConnection> &conn, int sessionId)
        : TcpSession(conn),
          m_id(sessionId),
          m_seq(0),
          m_isLogin(false) {
    m_userInfo.userId = 0;
    m_lastPackageTime = time(NULL);
}

void ChatSession::OnRead(const std::shared_ptr<TcpConnection> &conn, Buffer *pBuffer, Timestamp receivRime) {
    while (true)
    {
        //缓冲区中不足一个包，就直接返回
        if(pBuffer->readableBytes() < static_cast<size_t>(sizeof(KaHead_t)))
        {
            return;
        }
        KaHead_t head;
        memcpy(&head,pBuffer->peek(),sizeof(KaHead_t));
        if(head.flag == COMPRESSED)     //如果数据包已经压缩
        {
            if (head.compressSize <= 0 || head.compressSize > MAX_PACKAGESIZE ||
                head.originSize <= 0 || head.originSize > MAX_PACKAGESIZE) {
                conn->forceClose();
                //debug information
                return;
            }
            if (pBuffer->readableBytes() < static_cast<size_t>(head.compressSize) + sizeof(KaHead_t)) {
                return;
            }
            pBuffer->retrieve(sizeof(KaHead_t));
            std::string inBuf;
            inBuf.append(pBuffer->peek(),head.compressSize);
            pBuffer->retrieve(head.compressSize);
            std::string destBuf;
            if(!KaZlibUtil::uncompressBuf(inBuf,destBuf,head.originSize))
            {
                conn->forceClose();
                return;
            }
            if(!process(conn,destBuf.c_str(),destBuf.length()))
            {
                conn->forceClose();
                return;
            }
            m_lastPackageTime = time(NULL);
        }
        else                            //如果数据包没有压缩
        {
            if (head.originSize <= 0 || head.originSize > MAX_PACKAGESIZE) {
                conn->forceClose();
                return;
            }
            if (pBuffer->readableBytes() < static_cast<size_t>(head.originSize) + sizeof(KaHead_t)) {
                return;
            }
            pBuffer->retrieve(sizeof(KaHead_t));
            std::string inBuf;
            inBuf.append(pBuffer->peek(),head.originSize);
            pBuffer->retrieve(head.originSize);
            if(!process(conn,inBuf.c_str(),inBuf.length()))
            {
                conn->forceClose();
                return;
            }
            m_lastPackageTime = time(NULL);
        }   //end of else
    }//end of while loop
}


//process函数用于处理protobuf接收数据
bool ChatSession::process(const std::shared_ptr<TcpConnection> &conn, const char *buf, size_t buflength) {

}

