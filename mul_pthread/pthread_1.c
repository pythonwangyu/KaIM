#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* hello_mu(void*)
{
    printf("%s\n",__func__);
    return NULL;

}



int main()

{
    pthread_t tid;

    int ret ;
    ret = pthread_create(&tid,NULL,hello_mu,NULL);

    for(;;)
    {
        printf("%s\n",__func__);
        sleep(2);
    }
}

