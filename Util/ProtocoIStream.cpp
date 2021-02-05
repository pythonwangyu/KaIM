//
// Created by ka on 2021/1/19.
//

#include "ProtocoIStream.h"

using namespace KaNet;

unsigned short KaNet::checksum(const unsigned short *buffer, int size) {
    unsigned int cksum = 0;
    while(size > 1)
    {
        cksum += *buffer++;
        size -= sizeof(unsigned short);
    }
    if(size)
    {
        cksum += *(unsigned char*)buffer;
    }
    while(cksum >> 16)
    {
        cksum = (cksum >> 16) + (cksum & 0xffff);
    }
    return (unsigned short)(~cksum);
}

void KaNet::write7BitEncode(uint32_t value, std::string &buf) {
    do
    {
        unsigned char c = (unsigned char)(value & 0x7f);
        value >>= 7;
        if(value)
            c |= 0x80;
        buf.append(1,c);
    }while(value);
}

//写入7个字节
void KaNet::write7BitEncode(uint64_t value, std::string &buf) {
   do
   {
        unsigned char c = (unsigned char)(value &0x7f);
        value >>= 7;
        if(value)
            c |= 0x80;
        buf.append(1,c);
   }while(value);
}

