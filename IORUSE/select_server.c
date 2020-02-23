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
#include <ctype.h>

#define SERVER_PORT 6666


int main(int argc,char **argv) {
    
    int listenfd,connfd,sockfd;
    int nread;    //监听到的文件描述符，用于接受select的返回值。
    int i,j;
    int client[FD_SETSIZE];
    int maxi = -1;
    char buf[BUFSIZ];   //用于缓存读写的buff
    int n; //用于描述读写IO函数的返回值

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

        if(FD_ISSET(listenfd,&rdset))   //监听端口一次只能够接受一个连接。
        {
            client_len = sizeof(clientaddr);
            connfd = accept(listenfd,(struct sockaddr*)&clientaddr,&client_len);
            if(connfd < 0)   //判断是否accept成功！
            {
                perror("accept error");
                return -1;
            }

            for(i=0;i<FD_SETSIZE;i++)     //将监听到的连接添加到自己维护的文件描述符数组中。
                if(client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }

            if(FD_SETSIZE == i)    //判断是否超过连接的界限
            {
                perror("to much client\n");
                return -1;
            }

            FD_SET(connfd,&allset);  // 将sockfd加入监听的队列

            if(connfd > maxfd)    //增加监听的上限。
                maxfd = connfd;

            if(i > maxi)
                maxi = i;

            if(--nread == 0)    //倘若只有监听端口接收到读信号，就继续进行监听。
                continue;
        }

        //下面`一个for循环用于处理非监听窗口以外的窗口，判断其他的连接端口是否有数据的写入。
        //
        
        for(i=0;i<=maxi;++i)    //这里的逻辑思考清楚。是否需要加上等号。答案：必须加上等号。便于循环所有的自己管理client[FD_SETSIZE].\

        {
            if((sockfd = client[i]) < 0)
                continue;
            if(FD_ISSET(sockfd,&rdset))
            {
                if((n = read(sockfd,buf,sizeof(buf)))==0)
                {
                    if(close(sockfd))
                    {
                        perror("close error\n");
                        exit(-1);
                    }
                    FD_CLR(sockfd,&allset);
                    client[i] = -1;
                }
                else if (n > 0)
                {   //处理收到的数据.
                    for(j =0 ;j<n;++j)
                        buf[j] = toupper(buf[j]);  
                    write(sockfd,buf,n);
                    write(STDOUT_FILENO,buf,n);
                }

                if(--nread == 0)
                    break;
            }
        }
    }


    if(close(listenfd))
    {
        perror("close error\n");
        exit(-1);
    }
    return 0 ;
}
