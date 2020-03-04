#pragma once
#ifndef __ 

#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define MAX_EVENTS  1024                                    //监听上限数
#define BUFLEN 4096
#define SERV_PORT   8080


struct myevent_s {
    int fd;                                                 //要监听的文件描述符
    int events;                                             //对应的监听事件
    void *arg;                                              //泛型参数
    void (*call_back)(int fd, int events, void *arg);       //回调函数
    int status;                                             //是否在监听:1->在红黑树上(监听), 0->不在(不监听)
    char buf[BUFLEN];
    int len;
    long last_active;                                       //记录每次加入红黑树 g_efd 的时间值
};

int g_efd;                                                  //全局变量, 保存epoll_create返回的文件描述符
struct myevent_s g_events[MAX_EVENTS+1];                    //自定义结构体类型数组. +1-->listen fd
/*将结构体 myevent_s 成员变量 初始化*/
void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void *arg);

//通过自己定义的结构体数组对红黑树进行操作,上树操作。
void eventadd(int efd, int events, struct myevent_s *ev);

//将一个文件描述从树上摘除，并且将自己维护的结构体数组数据改变.
//其中状态的改变最为重要
void eventdel(int efd, struct myevent_s *ev);

//表示监听listenfd套接字接受一个连接，然后把该连接设置为指定的格式。
void acceptconn(int lfd, int events, void *arg);

//接受数据
void recvdata(int fd, int events, void *arg);

//发送数据
void senddata(int fd, int events, void *arg);


//初始化一个套接字，这里有一些疑惑，在这个函数中的套接字关闭？
//
void initlistensocket(int efd, short port);






#endif



