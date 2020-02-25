// File Name: Callfun_struct.c
// Author: kanon
// Created Time: 2020年02月25日 星期二 12时28分08秒
// Description:测试下结构体里面函数指针。

#include <stdio.h>
#include <unistd.h>


#define MAX_EVENT 1024

struct myevent{
    int status;
    char buf[100];
    void *arg;
    int id;
    void (*callback)(int ,char*,void *);
};

struct myevent g_event[MAX_EVENT+1];

void eventset(struct myevent *ev,int status,void (*callback)(int,char*,void *),void *arg)
{
    ev->status = status;
    ev->callback = callback;
    ev->arg = arg;
    ev->id = 0;

    return ;
}

void hello(int a,char*buf,void *arg)
{

    printf("%s\n",__func__);
}




void initevent(void)
{

    eventset(&g_event[MAX_EVENT],20,hello,&g_event[MAX_EVENT]);
    printf("%s\n",__func__);
    


}


int main(int argc,char **argv) {

    initevent();

    return 0 ;
}
