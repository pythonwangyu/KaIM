// File Name: select_server.c
// Author: kanon
// Created Time: 2020年02月21日 星期五 22时40分36秒

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <strings.h>


#define SERVER_PORT 6666


int main(int argc,char **argv) {
    
    int listenfd,connfd,sockfd;

    int i;
    int client[FD_SETSIZE];
    int maxi = -1;

    listenfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serveraddr,clientaddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVER_PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    listen(listenfd,128);

    fd_set allset,rdset;  //设置需要进行监听的文件描述符集合.
    



    





    return 0 ;
}
