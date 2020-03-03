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
    int listenfd,connfd,sockfd;
    struct sockaddr_in serveraddr,clientaddr;
    socklen_t len;
    char buf[BUFSIZE];
    char ip_str[INET_ADDRSTRLEN];   //用于接受和存储，从网络中来的IP地址（inet_ntop());

    listenfd = socket(AF_INET,SOCK_STREAM,0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;                 //端口复用
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
        int nfd = epoll_wait(efd,events,MAXEVENTS+1,-1);  //阻塞等待文件描述符处于就绪状态

        if(nfd < 0)
        {
            printf("epoll_wait error\n");
            return -1;
        }

        for(i=0;i<nfd;++i)
        {
            if(!(events[i].events & EPOLLIN))  //过滤掉非读事件
                continue;
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
                sockfd = events->data.fd;
                /* int readn = read(events[i].data.fd,buf,sizeof(buf)); */
                int readn = read(sockfd,buf,sizeof(buf));
                //通过readn来判断是否来连接!
                if(readn == 0)
                {
                    /* printf("peer closed\n"); */
                    /* epv.data.fd = events[i].data.fd; */
                    /* epv.events = EPOLLIN; */
                    /* ret = epoll_ctl(efd,EPOLL_CTL_DEL,epv.data.fd,&epv); */ 
                    /* close(sockfd); */

                    ret = epoll_ctl(efd,EPOLL_CTL_DEL,sockfd,NULL);  //从红黑树上摘除,这里可以使用空？
                    //为什么？传入信息不关心，不监听，直接删除，所以可以传入NULL；
                    if(ret < 0)
                    {
                        printf("从红黑树上删除失败\n");
                    }

                    close(sockfd);     //关闭这个端口,必须先摘除再关闭端口，不然会摘除失败
                    printf("peer [socket:%d] closed\n",sockfd);

                }
                else if (readn > 0)
                {
                    write(STDOUT_FILENO,buf,readn);
                    write(events->data.fd,buf,readn);
                }
                else    //读取出错也要从树中摘除,关闭套接字
                {
                    printf("read error\n");
                    ret = epoll_ctl(efd,EPOLL_CTL_DEL,sockfd,NULL);  //从红黑树上摘除,这里可以使用空？
                    //为什么？传入信息不关心，不监听，直接删除，所以可以传入NULL；
                    if(ret < 0)
                    {
                        printf("从红黑树上删除失败\n");
                    }
                    close(sockfd);
                }


            }

        }

    }
    return 0;
}




