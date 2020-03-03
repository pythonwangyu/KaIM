#### 注意点
（1）端口复用
为什么需要端口复用，因为当服务器先关闭的时候，会有一个TIMEWAIT时间.

```c
int opt = 1;

setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt);

```
