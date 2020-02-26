//description;某一个线程结束后返回的值通过pthread_exit(void *retval)返回，然后
//其他的线程可以使用pthread_join(pthread_t tid,void **ratval)来接受指定的线程的返回值。
//由于线程退出传出的值为void *retval类型，所以接收线程的接受类型为void **类型。
//
//实际上从线程中传出的实际上是一个指针，（范型指针）。

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct
{
   int a;
   int b;

}exit_t;


void *tfn(void *arg)
{
    exit_t *ret;
    ret = (exit_t *)malloc(sizeof(exit_t));
    
    ret->a = 100;
    ret->b = 300;
    pthread_exit((void*)ret);   //将ret指针传递出去，这个将被join接收。  //返回类型为：void *retval
}
void *myfun(void *arg)
{
    /* exit_t *retval = (exit_t *)arg; */
    /* retval->a =200; */
    /* retval->b = 500; */
    /* pthread_exit((void *)retval); */

    printf("%p\n",arg);
    exit_t *retval = (exit_t *)arg;
    printf("%p\n",retval);
    retval ->a =200;
    retval ->b =500;
    pthread_exit((void *)retval);
    


}

int main(void)
{
    pthread_t tid;
    

    exit_t *retval;   //使用retavl来接受传出的指针。

    //从线程中简单传出一个结构体数据指针.
    pthread_create(&tid,NULL,tfn,NULL);
    pthread_join(tid,(void **)&retval);  //阻塞等待，直到接受到线程结束的返回值的返回。这个传出参数为void **retval
    printf("a = %d,b = %d\n",retval->a,retval->b);
    free(retval);   //释放内存。


    //way two
    exit_t *ret = (exit_t *)malloc(sizeof(exit_t));
    printf("%p\n",ret);
    pthread_create(&tid,NULL,myfun,(void *)ret);
    pthread_join(tid,(void **)&ret);
    printf("a = %d,b = %d\n",ret->a,ret->b);
    printf("%p\n",(void **)&ret);

    free(ret);
    

    
    
    pthread_exit(NULL);



}
