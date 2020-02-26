#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *fun1(void *agc)
{
    printf("线程%d,runing\n",(int)agc);
    return NULL;
}

int main()
{
    pthread_t tid;
    int ret;
    int i;
    for(i=0;i<5;++i)
    {
        ret = pthread_create(&tid,NULL,fun1,(void*)i);
        
    }
    sleep(i);
    printf("i am the main_pthread,my pthread id = %lu\n",pthread_self());
    return 0;   //这里的return最好使用pthread_exit();因为上面的sleep没有的话，主控线程结束会返回给main,
                //那么整个进程就会结束。
}

