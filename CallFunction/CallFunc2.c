// File Name: CallFunc2.c
// Author: kanon
// Created Time: 2020年02月25日 星期二 00时30分40秒
// Description:  结构体中定义函数指针。这个指针变量直接指向某一个函数，这样就可以进行一些操作.
// 

#include <stdio.h>
#include <unistd.h>



struct myself
{
    int a;
    void *arg;
    void (*call_back)(int a,void *arg);
    
};


int del(int a,int b)
{
    if(a>b)
    {   printf("%s\n",__func__);
        return  a-b;
    }
    else
    {
        printf("%s\n",__func__);
        return -(b-a);
    }
}

int add(int a,int b)
{
    
    printf("%s\n",__func__);
    return (a+b);
}





int myself(int a,int b,int (*sum)(int x,int y))
{
    return sum(a,b);    
}

int main(int argc,char **argv) {

    myself(3,4,del);
    myself(3,4,add);

    return 0 ;
}
