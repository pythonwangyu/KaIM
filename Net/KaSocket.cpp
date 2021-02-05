//
// Created by ka on 2021/1/11.
//

#include "KaSocket.h"

#include <netinet/tcp.h>            //for TCP_NODELAY
#include <fcntl.h>                  //for fcntl  set socket fd as nonblock
#include <cstring>                  // for memset


using namespace  KaNet;


KaNetAddr::KaNetAddr()
{
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(static_cast<short>(PORT));
    m_addr.sin_addr.s_addr = inet_addr(IPADDR);
}
KaNetAddr::KaNetAddr(const char *ip, short port) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    m_addr.sin_addr.s_addr = inet_addr(ip);
}

KaNetAddr::KaNetAddr(std::string &ip, short port) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
}






//gloal function ,Bind() Listen(),Accept(),socket(),socketNoblock()

int KaNet::Bind(KaNetAddr& addr,socklen_t fd){
    int ret = bind(fd,addr.GetAddrPtr(),static_cast<socklen_t>(sizeof(addr.GetAddr_in())));
    return ret;
}

int KaNet::Listen(socket_t sockfd,int backlog){
    int ret = listen(sockfd,backlog);
    return ret;
}

int KaNet::Connect(socket_t fd,KaNetAddr& addr){
    int ret = connect(fd,addr.GetAddrPtr(),static_cast<socklen_t>(sizeof(addr.GetAddr_in())));
    return ret;
}

socket_t KaNet::Accept(socket_t sockfd,KaNetAddr& addr,socklen_t *addrlen){
    socket_t fd = accept(sockfd,addr.GetAddrPtr(),addrlen);
    return fd;
}
socket_t KaNet::Socket(){
    socket_t fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    return fd;
}

socket_t KaNet::SocketNoBlock(){
    socket_t fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    setSocketNoBlock(fd);
    return fd;
}

/*
 *  KaSocket memfunction implement
 *      //默认1生成的是阻塞的套接字，
 *
 *
 * */
KaSocket::KaSocket()
:m_fd(socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))
{
    //for debug
    std::cout << "KaSocket()" << std::endl;
}

KaSocket::~KaSocket() {
    ::close(m_fd);
    std::cout << "~KaSocket()" << std::endl;
}

int KaSocket::bindAddr(KaNetAddr &addr) const {
    int ret  = KaNet::Bind(addr,m_fd);
    std::cout << "绑定地址:" << addr.ToString() <<  std::endl;
    return ret;
}




int KaSocket::listen(int backlog) const {
    int ret = KaNet::Listen(m_fd,backlog);
    std::cout << "开启监听成功" << std::endl;
    return 0;
}

int KaSocket::connet(KaNetAddr &addr) const {
    int ret = KaNet::Connect(m_fd,addr);
    return ret;
}

socket_t KaSocket::accept(KaNetAddr &addr, socklen_t *addrlen) const {
    socket_t cfd = KaNet::Accept(m_fd,addr,addrlen);
    return cfd;
}

int KaSocket::setNonBlock(bool off) const {
    return KaNet::setSocketNoBlock(m_fd);
}

int KaSocket::setReuseAddr(bool off) const {
    return KaNet::setSocketReuseAddr(m_fd,off);
}

int KaSocket::setReusePort(bool off) const {
    return KaNet::setSocketReusePort(m_fd,off);
}

int KaSocket::setNoDelay(bool off) const {
    return KaNet::setSocketNoDelay(m_fd,off);
}

int KaSocket::setKeepAlive(bool off) const {
    return KaNet::setSocketKeeplive(m_fd,off);
}

int KaSocket::getError() const {
    return KaNet::getSocketError(m_fd);
}

void KaSocket::shutdownWrite() const {
    KaNet::shutdownWrite(m_fd);
}




/*
 * global's function for socket attribution
 * */


int KaNet::setSocketNoBlock(socket_t sock){
    int flags = ::fcntl(sock,F_GETFL,0);
    flags |= O_NONBLOCK;
    int ret = fcntl(sock,F_SETFL,flags);
    return ret;
}


int KaNet::setSocketNoDelay(socket_t sock,bool off){
    int optval = off?1:0;
    int ret  = ::setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,static_cast<void*>(&optval),
                            static_cast<socklen_t>(sizeof(optval)));
    return ret;
}

int KaNet::setSocketReuseAddr(socket_t sock,bool off){
    int optval = off?1:0;
    int ret = ::setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,static_cast<void*>(&optval),
                           static_cast<socklen_t>(sizeof(optval)));
    return ret;
}

int KaNet::setSocketReusePort(socket_t sock,bool off){
    int optval = off?1:0;
    int ret = ::setsockopt(sock,SOL_SOCKET,SO_REUSEPORT,static_cast<void*>(&optval),
                           static_cast<socklen_t>(sizeof(optval)));
    return ret;
}


int KaNet::setSocketKeeplive(socket_t sock,bool off){
    int optval = off?1:0;
    int ret = ::setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,static_cast<void*>(&optval),
                           static_cast<socklen_t>(sizeof(optval)));
    return ret;
}


int KaNet::getSocketError(socket_t fd){
    int optval = 0;
    auto optlen = static_cast<socklen_t>(sizeof(optval));
    if(::getsockopt(fd,SOL_SOCKET,SO_ERROR,static_cast<void*>(&optval),&optlen)){
        return errno;
    }
    return optval;
}


void KaNet::close(socket_t sockfd){
#ifdef WIN32

#else
    ::close(sockfd);
#endif
}

void KaNet::shutdownWrite(int sockfd) {
    ::shutdown(sockfd,SHUT_WR);
}

//获取对端或者本端的struct sockaddr_in

//wrap getsockname
struct sockaddr_in KaNet::getLocalAddr(socket_t sockfd){
    struct sockaddr_in localaddr = {0};
    memset(&localaddr,0,sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    ::getsockname(sockfd,reinterpret_cast<struct sockaddr*>(&localaddr),&addrlen);
    return localaddr;
}


// wrap getpeername
struct sockaddr_in KaNet::getPeerAddr(socket_t sockfd){
    struct sockaddr_in peeraddr = {0};
    memset(&peeraddr,0,sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    ::getpeername(sockfd,reinterpret_cast<struct sockaddr*>(&peeraddr),&addrlen);
    return peeraddr;
}