//
// Created by ka on 2021/1/13.
//


#ifndef __KANET_PLATFROM_H__
#define __KANET_PLATFROM_H__

#include <cstdint>

//for socket fd;
#define socket_t int


#ifdef WIN32
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Shlwapi.lib")

typedef int socklen_t;
typedef unsigned int in_addr_t;

#define POLLIN      1
#define POLLPRI     2
#define POLLOUT     4
#define POLLERRPR   8
#define POLLHUP     16

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3



#pragma pack(push,1)

typedef union epoll_data{
    void* ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
}epoll_data_t;


struct epoll_event{
    uint32_t events;
    epoll_data_t data;
};

#pragma pack(pop)


#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <fcntl.h>


class NetWorkInitializer{
public:
    NetworkInitializer();
    ~NetworkInitializer();
};

NetworkInitializer::NetworkInitializer(){
    WORD Version = MAKEWORD(2,2);
    WSADATA wsaData;
    ::WSADtartup(Version,&wsaData);
}

NetworkInitializer::~NetworkInitializer(){
    ::WSACleanup();
}



#else

/*for Linux plafrom*/
#include <sys/epoll.h>
#include <unistd.h>




#endif  //for condition complier


#endif //KANET_PLATFROM_H
