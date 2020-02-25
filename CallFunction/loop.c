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


void recvdata(int fd,int events,void *arg);
void senddata(int fd,int events,void *arg);

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


void acceptconn(int lfd,int events,void *arg);


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

        
}





void initlistensocket(int efd,short port)
{
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    fcntl(lfd,F_SETFL,O_NONBLOCK);   //将socket设为非阻塞。
    
    //将全局自定义结构体数组进行维护，将最后一个结构体数组元素设置为初值。
    eventset(&g_events[MAX_EVENTs],lfd,acceptconn,&g_events[MAX_EVENTs]);

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
    struct epoll_event epv = {0,{0}};
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

void acceptconn(int lfd,int events,void *arg)
{

}









