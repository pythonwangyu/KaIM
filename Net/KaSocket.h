//
// Created by ka on 2021/1/11.
//

#ifndef KANET_KASOCKET_H
#define KANET_KASOCKET_H

#include "../Util/Platfrom.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>


#include <string>
#include <iostream>



#define IPADDR  "127.0.0.1"
#define PORT    88888





namespace KaNet{

    class KaNetAddr;


    /*
     *
     * basical class wrap socket_t  in order to simple the operation of socket function
     *  member :
     *          m_fd        stands for the socket fd;
     *
     * */
    class KaSocket{
    public:
        explicit KaSocket();
        explicit KaSocket(socket_t sockfd):m_fd(sockfd){}

        ~KaSocket();


        //inner socket function



        int bindAddr(KaNetAddr& addr) const;

        int listen(int backlog) const;

        int connet(KaNetAddr& addr) const;

        socket_t accept(KaNetAddr& addr,socklen_t* addrlen) const;


        /*
         * set socket attribution
         * */

        int setNonBlock(bool off) const;

        int setReuseAddr(bool off) const;

        int setReusePort(bool off) const;

        int setKeepAlive(bool off) const;

        int setNoDelay(bool off) const;



        void shutdownWrite() const;


        //more detail function

        int getError() const;


        socket_t fd(){
            return m_fd;
        }


    private:
        socket_t m_fd;

    };



    /*
     *  wrap the struct sockaddr_in,make bind() accept() connect()  more easy to use ,and also can help debug
     *  stringform the ip and port
     * */
    class KaNetAddr{
    public:

        /*default port by marco*/
        explicit KaNetAddr();

        /* accept string and port */
        explicit KaNetAddr(std::string &ip,short port);

        explicit KaNetAddr(const char* ip,short port);

        explicit KaNetAddr(const struct sockaddr_in& addr)
                :m_addr(addr)
                {}


        /*采用了最危险的强制类型转换*/
        struct sockaddr* GetAddrPtr(){
            return reinterpret_cast<struct sockaddr*>(&m_addr);
        }

        struct sockaddr_in GetAddr_in(){
            return m_addr;
        }

        struct sockaddr_in* GetAddr_int_Ptr(){
            return &m_addr;
        }





        //for debug
        std::string ToString(){
            char buf[512] = {0};
            snprintf(buf,sizeof(buf),"IP:%d,Port=%d",m_addr.sin_addr.s_addr,m_addr.sin_port);
            std::string out(buf);
            return out;
        }


    private:
        struct sockaddr_in  m_addr{};                 //need the head file  #incldue <apra/inet.h>

    };


    //wrap the socket function

    /*
     *  Get the block socket
     * */
    socket_t Socket();

    /*
     *  Get Noblock socket
     * */
    socket_t SocketNoBlock();

    /*
     *  set fd noblock if off == true,
     *  else make socket block
     * */

    int setSocketNoBlock(socket_t sock);

    int setSocketNoDelay(socket_t sock,bool off);

    int setSocketReuseAddr(socket_t sock,bool off);

    int setSocketReusePort(socket_t sock,bool off);

    int setSocketKeeplive(socket_t sock,bool off);

    /*
     *
     * bind
     *
     * listen
     *
     * connect
     *
     * accept
     *
     *
     * */
    int Bind(KaNetAddr& addr,socklen_t fd);

    int Listen(socket_t sockfd,int backlog);

    int Connect(socket_t fd,KaNetAddr& addr);

    socket_t Accept(socket_t sockfd,KaNetAddr& addr,socklen_t *addrlen);


    //more useful function for socket fd.
    int getSocketError(socket_t fd);

    //強制關閉,
    void close(socket_t sockfd);

    //優雅關閉
    void shutdownWrite(socket_t sockfd);


    //獲取對端和本端口的sockaddr_in

    struct sockaddr_in getLocalAddr(socket_t sockfd);

    struct sockaddr_in getPeerAddr(socket_t sockfd);




}       //end of KaNet namespace


#endif //KANET_KASOCKET_H
