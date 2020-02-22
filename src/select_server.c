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
    int nread;    //监听到的文件描述符，用于接受select的返回值。
    int i;
    int client[FD_SETSIZE];
    int maxi = -1;

    listenfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serveraddr,clientaddr;
    socklen_t client_len;

    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVER_PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    listen(listenfd,128);

    fd_set allset,rdset;  //设置需要进行监听的文件描述符集合.
    int maxfd;
    maxfd = listenfd;
    FD_ZERO(&allset);
    FD_SET(listenfd,&allset);

    for(i=0;i<FD_SETSIZE;++i)
        client[i] = -1;

    for(;;)
    {
        rdset = allset;
        nread = select(maxfd+1,&rdset,NULL,NULL,NULL);
        if(FD_ISSET(listenfd,&rdset))
        {
            client_len = sizeof(clientaddr);
            sockfd = accept(listenfd,(struct sockaddr*)&clientaddr,&client_len);
            if(sockfd < 0)   //判断是否accept成功！
            {
                perror("accept error");
                return -1;
            }

            for(i=0;i<FD_SETSIZE;i++)
                if(client[i] < 0)
                {
                    client[i] = sockfd;
                    break;
                }
            if(FD_SETSIZE == i)
            {
                perror("to much client\n");
                return -1;
            }

            if(sockfd > maxfd)    //增加监听的上限。
                maxfd = sockfd;
            
            FD_SET(sockfd,&allset);  // 将sockfd加入监听的队列

            if(--nread == 0)    //倘若只有监听端口接收到读信号，就继续进行监听。
                continue;
        }
        for(i=0;i<=maxi;++i)
        {

        }

    }



    





    return 0 ;
}
