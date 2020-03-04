#### epoll_loop介绍
- 1.三个回调函数
（1）acceptconn()
    监听描述符号专用的回调函数，一旦监听函数接收到连接请求，就会执行。


在main函数中有这么几个重要的点
```c
nfd = epoll_wait(g_efd,events,MAX_EVENTS+1,1000);

for(int i =0;i<nfd;i++)
{
    //自定义的结构体myevent_s类型指针，接收联合体data里面的*ptr的数据，
    //通过这些数据进行访问。
    struct myevent_s *ep = (struct myevent_s *)events[i].data.ptr;

    //进入判定，调用回调函数
    if((events[i].events & EPOLLIN) && (ev->events & EPOLLIN))   //读事件就绪，调用对应回调函数
    {
        ev->call_back(ev->fd,events[i].events,ev->arg);
    }

    if((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))  //写事件就绪，调用对应回调函数。
    {
        ev->call_back(ev->fd,events[i].events,ev->arg);
    }

}
```
- 2.关于文件符的问题

（1）在被调用的函数中创建了一个文件描述符，不传出，那么这个文件描述符将会一直存在吗？这个文件描述符如果存在会
该怎么消除？

（2）文件描述符不关闭会怎样？一直占据吗？是不是必须进行关闭。














