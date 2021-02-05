//
// Created by ka on 2021/1/18.
//

#include "TcpSessin.h"
#include "../Util/KaZlibUtil.h"
#include "../MsgDefine/MsgDefine.h"

using namespace KaNet;


TcpSession::TcpSession(const std::weak_ptr<TcpConnection> &tempconn)
    :m_tmpConn(tempconn)
{}





//
////业务逻辑,实际发送数据包的内容。，这个send函数之前需要将数据指定发送
//void TcpSession::sendPack(const char *p, int32_t length) {
//    string srcbuf(p,length);        //将char*数据打包到p中
//    string destbuf;
//    if(!KaZlibUtil::compressBuf(srcbuf,destbuf))
//    {
//        //fix up .use Log
//        std::cout << "compress buf error" << std::endl;
//        return;
//    }
//    string PackageData;
//    //chat_msg_header header;
//    header.compressflag = 1;
//    header.compresssize = destbuf.length();
//    header.originsize = length;
//    PackageData.append((const char*)&header,sizeof (header));
//    PackageData.append(destbuf);
//    if(m_tmpConn.expired())
//    {
//        //Log
//        return;
//    }
//    std::shared_ptr<TcpConnection> conn =m_tmpConn.lock();
//    if(conn)
//    {
//        conn->send(PackageData);
//    }
//}