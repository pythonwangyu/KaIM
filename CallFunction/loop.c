// File Name: loop.c
// Author: kanon
// Created Time: 2020年02月25日 星期二 13时10分42秒
// Description:

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define MAX_EVENTs 1024
#define BUFLEN 1096
#define SERV_PORT 8080



struct myevent_s{
    int fd;
    int events;
    void *arg;
    void (*call_back)(int fd,int events,void *agc);
    int status;
    char buf[BUFLEN];
    int len;
    long last_active;
};

int g_efd;
struct myevent_s g_events[MAX_EVENTs+1];

void initlistensocket(int efd,short port);

void eventset(struct myevent_s *ev,int fd,void (*call_back)(int,int,void*),void *arg);

void eventadd(int efd,int events,struct myevent_s *ev);

void eventdel(int efd,struct myevent_s *ev);

void acceptconn(int lfd,int events,void *arg);

void recvdata(int fd,int events,void *arg);

void senddata(int fd,int events,void *arg);

int main(int argc,char *argv[])
{
    unsigned short port = SERV_PORT;

    if (argc == 2)
        port = atoi(argv[1]);

    g_efd = epoll_create(MAX_EVENTs+1);
    if(g_efd <= 0)
    {
        printf("create efd in %s err %s\n",__func__,strerror(errno));
        exit(-1);
    }
    
    initlistensocket(g_efd,port);

    struct epoll_event events[MAX_EVENTs+1];
    printf("server running:port[%d]\n",port);

    int checkpos  = 0,i;

    while(1)
    {//超时验证逻辑


        int nfd = epoll_wait(g_efd,events,MAX_EVENTs+1,1000);
        if(nfd <0)
        {
            printf("epoll_wait error,exit\n");
            break;
        }
        for(i=0;i<nfd;i++)
        {
            struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;

            if((events[i].events &EPOLLIN) && (ev->events & EPOLLIN))
            {
                ev->call_back(ev->fd,events[i].events,ev->arg);
            }

            //Do something...
            //
            
            if((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
            {
                ev->call_back(ev->fd,events[i].events,ev->arg);
            }

        }
    }
    return 0;

        
}

void initlistensocket(int efd,short port)
{
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    fcntl(lfd,F_SETFL,O_NONBLOCK);   //将socket设为非阻塞。
    
    //将全局自定义结构体数组进行维护，将最后一个结构体数组元素设置为监听端口，并且初始化。
    eventset(&g_events[MAX_EVENTs],lfd,acceptconn,&g_events[MAX_EVENTs]);

    //将lfd添加到红黑树上，得出结论。使用的是结构体数组中元素进行初始化
    eventadd(efd,EPOLLIN,&g_events[MAX_EVENTs]);

    int opt = 1;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));  //IO端口复用.

    struct sockaddr_in sin;
    memset(&sin,0,sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);

    bind(lfd,(struct sockaddr *)&sin,sizeof(sin));
    listen(lfd,20);

    return;
}


void eventset(struct myevent_s *ev,int fd,void (*call_back)(int,int,void*),void *arg)
{
    ev->fd = fd;
    ev->call_back = call_back;
    ev->events = 0;
    ev->arg = arg;
    ev->status = 0;
    memset(ev->buf,0,sizeof(ev->buf));
    ev->len = 0;
    ev->last_active = time(NULL);
    return;

}


void eventadd(int efd,int events,struct myevent_s *ev)
{
    struct epoll_event epv = {0,{0}};  //内部的临时变量,不是的自定义的，是epoll_event结构体
    int op;
    epv.data.ptr = ev;
    epv.events = ev->events = events;

    if(ev->status == 1)
        op = EPOLL_CTL_MOD;
    else
    {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }
    if(epoll_ctl(efd,op,ev->fd,&epv) < 0)
        printf("event add err [fd = %d],events[%d]\n",ev->fd,events);
    else
        printf("event add OK [fd = %d],op=%d,events[%0X]\n",ev->fd,op,events);

    return;
}

void eventdel(int efd,struct myevent_s *ev)
{
    struct epoll_event epv = {0,{0}};
    if(ev->status!=1)
        return;
    epv.data.ptr = ev;
    ev->status = 0;
    epoll_ctl(efd,EPOLL_CTL_DEL,ev->fd,&epv);

    return;
}

void acceptconn(int lfd,int events,void *arg)
{
    struct sockaddr_in cin;
    socklen_t len = sizeof(cin);
    int cfd,i;

    if((cfd = accept(lfd,(struct sockaddr*)&cin,&len)) == -1)
    {
        printf("%s:accept,%s\n",__func__,strerror(errno));
        return;
    }

    do
    {
        for(i=0;i<MAX_EVENTs;i++)
            if(g_events[i].status == 0)
                break;
        if(i == MAX_EVENTs)
        {
            printf("%s:max connect limit[%d]\n",__func__,MAX_EVENTs);
            break;
        }
        int flag = 0;
        if((flag = fcntl(cfd,F_SETFL,O_NONBLOCK))<0)
        {
            printf("%s:fcntl nonblocking failed,%s\n",__func__,strerror(errno));
            break;
        }

        eventset(&g_events[i],cfd,recvdata,&g_events[i]);
        eventadd(g_efd,EPOLLIN,&g_events[i]);

    }while(0);

}

void recvdata(int fd,int events,void *arg)
{
    struct myevent_s *ev = (struct myevent_s*)arg;
    int len;
    len = recv(fd,ev->buf,sizeof(ev->buf),0);
    eventdel(g_efd,ev);   //将该节点从红黑树上摘除

    if(len>0)
    {
        ev->len = len;
        ev->buf[len] = '\0';
        printf("C[%d]:%s\n",fd,ev->buf);

        eventset(ev,fd,senddata,ev);
        eventadd(g_efd,EPOLLOUT,ev);
    }
    else if(len == 0)
    {
        close(ev->fd);
        printf("[fd=%d] pos[%ld],closed\n",fd,ev-g_events);
    }
    else
    {
        close(ev->fd);
        printf("recv[fd=%d] error[%d]:%s\n",fd,errno,strerror(errno));
    }
    return;
}


void senddata(int fd,int events,void *arg)
{
    struct myevent_s *ev = (struct myevent_s*)arg;
    int len;
    len = send(fd,ev->buf,ev->len,0);
    if(len > 0)
    {
        printf("send[fd=%d],[%d]%s\n",fd,len,ev->buf);
        eventdel(g_efd,ev);
        eventset(ev,fd,recvdata,ev);
        eventadd(g_efd,EPOLLIN,ev);
    }
    else
    {
        close(ev->fd);
        eventdel(g_efd,ev);
        printf("send[fd= %d] error %s\n",fd,strerror(errno));
    }
    return;
}



