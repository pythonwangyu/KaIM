#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* tfn(void* arg)
{
    printf("%s\n",__func__);
    return NULL;
}



int main()

{
    pthread_t tid;

    int ret ;
    ret = pthread_create(&tid,NULL,tfn,NULL);

    for(;;)
    {
        printf("%s\n",__func__);
        sleep(2);
    }
    return 0;
}

