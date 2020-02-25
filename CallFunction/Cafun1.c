// File Name: Cafun1.c
// Author: kanon
// Created Time: 2020年02月24日 星期一 18时44分35秒

#include <stdio.h>
#include <unistd.h>

void teat001()
{
    printf("%s\n",__func__);
}


void print_hi(void (*myfunc)())
{
    printf("%s\n",__func__);
}


int test01(int a,char b)
{
    printf("%s\n",__func__);
}


int main(int argc,char **argv) {

    /* printf("函数的入口：%p\n",teat001); */
    //函数名字是一个指针！
    
    void (*myfunc)() = teat001;
    myfunc();
    print_hi(teat001);



    return 0 ;
}
