#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define PORT 8080
#define MAXEVENTS 1024
#define BUFSIZE 2048

int main(int argc,char* argv[])
{
    int listenfd,connfd;
    struct sockaddr_in serveraddr,clientaddr;
    socklen_t len;
    char buf[BUFSIZE];
    char ip_str[INET_ADDRSTRLEN];

    listenfd = socket(AF_INET,SOCK_STREAM,0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    
    bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    
    listen(listenfd,128);

    struct epoll_event epv,events[MAXEVENTS];
    epv.data.fd = listenfd;
    epv.events = EPOLLIN;
    int efd = epoll_create(MAXEVENTS);
    int ret,i;
    
    ret = epoll_ctl(efd,EPOLL_CTL_ADD,listenfd,&epv);

    if(ret < 0)
    {
        printf("epoll_ctl error\n");
        return -1;
    }
    while(1)
    {
        len = sizeof(clientaddr);
        int nfd = epoll_wait(efd,events,MAXEVENTS+1,-1);
        if(nfd < 0)
        {
            printf("epoll_wait error\n");
            return -1;
        }
        for(i=0;i<nfd;++i)
        {
            if((events[i].events & EPOLLIN) && (events[i].data.fd == listenfd))
            {
                connfd = accept(listenfd,(struct sockaddr*)&clientaddr,&len);
                printf("接收到来自IP：%s client connect at port:%d\n",
                       inet_ntop(AF_INET,&clientaddr.sin_addr,ip_str,sizeof(ip_str)),
                       ntohs(clientaddr.sin_port));
                epv.events = EPOLLIN;
                epv.data.fd = connfd;
                epoll_ctl(efd,EPOLL_CTL_ADD,connfd,&epv);
            }
            else if (events[i].events & EPOLLIN)
            {
                int readn = read(events[i].data.fd,buf,sizeof(buf));
                //通过readn来判断是否来连接!
                if(readn == 0)
                {
                    printf("peer closed\n");
                    epv.data.fd = events[i].data.fd;
                    epv.events = EPOLLIN;
                    ret = epoll_ctl(efd,EPOLL_CTL_DEL,epv.data.fd,&epv);
                    if(ret < 0)
                    {
                        printf("从红黑树上删除失败\n");
                    }
                }
                else if (readn > 0)
                {
                    write(STDOUT_FILENO,buf,readn);
                    write(events->data.fd,buf,readn);
                }
                else
                {
                    printf("read error\n");
                    return -1;
                }


            }

        }

    }
    return 0;
}




