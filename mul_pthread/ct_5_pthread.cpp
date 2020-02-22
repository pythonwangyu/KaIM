// File Name: ct_5_pthread.cpp
// Author: kanon
// Created Time: 2020年02月22日 星期六 18时39分28秒

#include <iostream>
#include <unistd.h>
#include <pthread.h>


#define NUM_PTHREAD 5

using namespace std;

void *printfhi(void* argvs)
{
    printf("hi i am pthread\n");
    return 0;
}

int main(int argc,char **argv) {

    pthread_t   tids[NUM_PTHREAD];
    for(int i=0;i<NUM_PTHREAD;++i)
    {
        int ret = pthread_create(&tids[i],NULL,printfhi,NULL);
        if(0 != ret)
        {
            cout<<"pthread_create error,error code:"<<ret<<"!"<<endl;
        }
    }

    printf("线程结束了，我也结束了\n");
    pthread_exit(NULL);
    return 0;
}
