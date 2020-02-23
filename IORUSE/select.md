#### select函数
- 作用:synchronous IO multiplexing(同步IO复用)
    
```c
select(maxfd,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,struct timeval timeout);
其中，maxfd为最大的文件描述符加一
struct timeval
{
    long tv_sec;
    long tv_usec;
};

struct timesepc
{
    long tv_sec;
    long tv_usec;
};

if timeout is NULL,select() can block indefinitely.
```
- 操作fd_set的几个函数

```c
//fd 是否在监听队列set中
FD_ISSET(int fd, fd_set *set);

//将fd从监听集合set中删除
FD_CLR(int fd,fd_set *set)

//将监听集合set中的描述符清空
FD_ZERO(fd_set *fd);

//将fd设置到监听集合中
FD_SET(int fd,fd_set *set);

```


- select server logic

一般需要三个文件描述符:listenfd,connfd,sockfd;

(1)listenfd::专用的监听描述符。

(2)connfd::用于接受连接上服务器的连接。

(3)sockfd::用于处理已经连接的端口的信息接受。

```bash
步骤：
1.创建监听文件描述符
    int listenfd = socket()  ->   bind()  -> listen()  -> 

2.创建自己维护的客户端数组、读监听队列（监听队列、错误处理队列）
    client[FD_SETSIZE] && int maxi = -1 ->maxfd = listenfd;   
    fd_set readset,allset;
    FD_ZERO(&allset);
    FD_SET(listenfd,&allset);

3.创建while(1)循环

    1.readfd = allfd;
    2.nread = select(maxfd,&readfd,NULL,NULL,NULL);
    3   if语句
                监听端口持续监听判断

    4   for-i 循环语句
                处理已经连接端口的信息
    



4.退出循环后，关闭监听端口的文件描述符listenfd
    


```












