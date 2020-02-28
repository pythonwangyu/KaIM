#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct msg{
    int num;
    struct msg *next;
};

struct msg *head = NULL;
struct msg *mp = NULL;

//利用宏初始化条件变量与互斥锁。
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t has_product = PTHREAD_COND_INITIALIZER;

void *producter(void *arg)
{
    while (1) {
        mp = (struct msg*)malloc(sizeof(struct msg));
        mp->num = rand() % 400 + 1;
        printf("---producted---%d\n", mp->num);     //生产数据，在堆区中增加需要的内存变量。

        pthread_mutex_lock(&mutex);   //给数据中间添加数据的时候应该加锁。
        mp->next = head;
        head = mp;
        pthread_mutex_unlock(&mutex);
        //生产好之后，唤醒条件变量。
        pthread_cond_signal(&has_product);
        sleep(rand() % 3);
    }

    return NULL;
}

void *consumer(void *arg)
{
    while (1) {
        pthread_mutex_lock(&mutex);   //判断条件变量之前，需要抢夺到条件变量（锁）。
        while (head == NULL) {
            pthread_cond_wait(&has_product, &mutex);  //这个有两个步骤，但属于一个原子操作。（1）阻塞等待（2）解锁互斥量
        }                                           //被唤醒后，函数返回，解除阻塞，重新获取互斥锁。
        mp = head;              //继续自己要进行的操作
        head = mp->next;
        pthread_mutex_unlock(&mutex);    //完成操作后，解除互斥锁。

        printf("------------------consumer--%d\n", mp->num);
        free(mp);
        mp = NULL;
        sleep(rand() % 3);
    }

    return NULL;
}

int main(void)
{
    pthread_t ptid, ctid;


    pthread_create(&ptid, NULL, producter, NULL);
    pthread_create(&ctid, NULL, consumer, NULL);

    pthread_join(ptid, NULL);
    pthread_join(ctid, NULL);

    return 0;
}
