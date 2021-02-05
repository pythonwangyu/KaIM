//
// Created by ka on 2021/1/15.
//

#include "DaemonRun.h"
#include <unistd.h>
#include <iostream>
#include <csignal>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//
void daemon_run()
{
    int pid;
    signal(SIGCHLD,SIG_IGN);
    pid = fork();
    if(pid < 0)
    {
        std::cout << "fork error" << std::endl;
        exit(-1);
    }
    else if(pid > 0)
    {
        exit(0);        //父进程退出
    }
    ::setsid();       //child将重新获取一个新的会话

    //dup函数重新定义输出输入端口。
    int fd = open("/dev/null",O_RDWR,0);
    if(fd != -1)
    {
        ::dup2(fd,STDIN_FILENO);
        ::dup2(fd,STDOUT_FILENO);
        ::dup2(fd,STDERR_FILENO);
    }
    if(fd > 2)          //关闭套接字
    {
        ::close(fd);
    }

}
