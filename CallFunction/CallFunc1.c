// File Name: CallFunc1.c
// Author: kanon
// Created Time: 2020年02月24日 星期一 23时30分28秒

#include <stdio.h>
#include <unistd.h>


//理解函数指针，指的是指向函数的指针。
//格式：必须加上括号，不然前面的*就会成为返回值的一员
// return_type (* function_name)(typename1 parame1, typename2 parame2......);
// 可以使用typedef将函数指针定义为一种数据类型


int CountAge(int num)
{
    if(num <=1)
    {
        return 5;
    }
    return 1+CountAge(num -1);
}

int mysum(int a,int b)
{
    printf("%s\n",__func__);  //调用此函数的时候会打印出本函数的名字！
    return (a+b);
}

int main(int argc,char **argv) {

    /* int (*son)(int num);   //定义了一个函数指针 */
    /* son = CountAge;       //函数指针变量son指向CountAge()函数地址. */

    /* int (*Plusnum)(int,int) = mysum; */
    typedef int (*PLUSNUM)(int,int);

    PLUSNUM plusnum = mysum;

    printf("%d\n",plusnum(40,10));






    return 0 ;
}
