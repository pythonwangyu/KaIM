//
// Created by ka on 2021/1/22.
//

#ifndef KANET_MSGDEFINE_H
#define KANET_MSGDEFINE_H

#include <cstdint>

//是否压缩的标志
enum COMPRESS_FLAG{
    UNCOMPRESSED,         //未压缩
    COMPRESSED            //压缩
};

//信息的条件？
enum MainCMD{
    SID_LOGIN           = 0x0001,           // for login
    SID_BUDDY_LIST      = 0x0002,		    // for friend list
    SID_MSG             = 0x0003,           //
    SID_GROUP           = 0x0004,           // for group message
    SID_FILE            = 0x0005,
    SID_SWITCH_SERVICE  = 0x0006,
    SID_OTHER           = 0x0007,
    SID_INTERNAL        = 0x0008
};

//1字节对齐，节省内存
#pragma pack(push, 1)
//协议头
struct KaHead_t
{
    COMPRESS_FLAG flag;         //压缩标志？
    int32_t  originSize;       //包体压缩前大小
    int32_t  compressSize;     //包体压缩后大小
    MainCMD  mainCMD;           //决定后面的protobuf的结构体，主命令选择
    uint32_t seq;               //序列
};
#pragma pack(pop)






#endif //KANET_MSGDEFINE_H
