#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define false 0
#define true 1

typedef struct
{
    void*(*function)(void*);
    void* arg;
}pthread_task_t;

struct pthreadpool_t
{
    pthread_mutex_t lock;
    pthread_mutex_t pthread_counter;
    pthread_cond_t queue_not_full;
    pthread_cond_t queue_not_empty;

    pthread_t *pthreads;                  /*保存线程Id的数组*/
    pthread_t Adjust_Tid;                /*管理者线程id*/   
    pthread_task_t *task_queue;         /*任务队列,包含函数指针结构体数组*/

    int min_pthr_num;                    
    int max_pthr_num;
    int live_pthr_num;                   /*当前存活线程数*/    
    int busy_pthr_num;                   /*繁忙线程数目*/
    int wait_pexit_thr_num;              /*即将销毁的线程数目*/

    int queue_front;
    int queue_rear;
    int queue_size;
    int queue_max_size;

    int shutdown;                       /*线程池的使用情况*/
};

//工作线程
void* threadpool_thread(void *pthreadpool)
{
    pthreadpool_t *pool = (pthreadpool_t*)pthreadpool;
    pthread_task_t task;

    while(true)
    {
        pthread_mutex_lock(&(pool->lock));  //锁住整个结构体。


        while((pool->queue_size == 0) && (!pool->shutdown))   //当任务队列为空，并且不关闭进入循环
        {
            printf("pthread %lu is waiting\n",pthread_self());
            pthread_cond_wait(&(pool->queue_not_empty),&(pool->lock));   //线程阻塞阻塞等待，等待队列不为空的条件变量。

            if(pool->wait_pexit_thr_num > 0)
            {
                pool->wait_pexit_thr_num--;

                //如果线程池中的线程个数大于最小值时，可以结束当前线程?
                if(pool->live_pthr_num > pool->min_pthr_num)
                {
                    printf("pthread %lu is exiting\n",pthread_self());
                    pool->live_pthr_num--;
                    pthread_mutex_unlock(&(pool->lock));
                    pthread_exit(NULL);
                }
            }
        }


        if(pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->lock));
            printf("pthread %lu is exiting\n",pthread_self());
            pthread_exit(NULL);
        }


        //有任务会执行以下语句
        task.function = pool->task_queue[pool->queue_front].function;
        task.arg = pool->task_queue[pool->queue_front].arg;

        pool->queue_front = (pool->queue_front+1) % pool->queue_max_size;    //任务被去走后，队列的头指针会改变。
        pool->queue_size--;                                                  //队列的尺寸会变小

        //唤醒服务器的接收端口，让使得任务队列可以被添加任务。
        pthread_cond_broadcast(&(pool->queue_not_full));

        pthread_mutex_unlock(&(pool->lock));   //解锁

        //执行任务
        printf("pthread %lu start working\n",pthread_self());
        pthread_mutex_lock(&(pool->pthread_counter));    //忙进程加锁.
        pool->busy_pthr_num++;
        pthread_mutex_unlock(&(pool->pthread_counter));  //立即解锁

        (*(task.function))(task.arg);   //执行回调函数任务

        printf("pthread %lu end working\n",pthread_self());
        pthread_mutex_lock(&(pool->pthread_counter));
        pool->busy_pthr_num--;
        pthread_mutex_unlock(&(pool->pthread_counter));
    }

    pthread_exit(NULL);
    
}




//创建一个线程池
pthreadpool_t *pthreadpool_create(int min_pthr_num,int max_pthr_num,int queue_max_size)
{
    int i;
    pthreadpool_t *pool = NULL;
    do
    {
        if((pool = (pthreadpool_t *)malloc(sizeof(pthreadpool_t))) == NULL)
        {
            printf("malloc pthreadpool error\n");
            break;
        }

        pool->min_pthr_num = min_pthr_num;
        pool->max_pthr_num = max_pthr_num;
        pool->busy_pthr_num = 0;
        pool->live_pthr_num = min_pthr_num;    //活着的线程数目就是最初的最小线程值。

        pool->queue_size = 0;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->shutdown = false;

        pool->pthreads = (pthread_t *)malloc(sizeof(pthread_t)*max_pthr_num);  //开辟出存放线程id的数组
        if(pool->pthreads == NULL)
        {
            printf("malloc thread error\n");
            break;
        }
        
        //任务队列开辟空间
        pool->task_queue = (pthread_task_t *)malloc(sizeof(pthread_task_t)*pool->queue_max_size);
        if(pool->task_queue == NULL)
        {
            printf("malloc task_queue fail\n");
            break;
        }

        //初始化互斥量条件变量
        if(pthread_mutex_init(&(pool->lock),NULL) != 0 
           || pthread_mutex_init(&(pool->pthread_counter),NULL) != 0
           || pthread_cond_init(&(pool->queue_not_empty),NULL) != 0
           || pthread_cond_init(&(pool->queue_not_full),NULL) != 0)
        {
            printf("init lock or cond error\n");
            break;
        }
        
        //启动min_thr_num 个work pthread
        for(int i=0;i<min_pthr_num;++i)
        {
            pthread_create(&(pool->pthreads[i]),NULL,threadpool_thread,(void*)pool);
            printf("start thread %lu......\n",pool->pthreads[i]);

        }
        pthread_create(&(pool->Adjust_Tid),NULL,adjust_thread,(void *)pool);

        return pool;


    }while(0);
    pthreadpool_free(pool);
}


int pthreadpool_add(pthreadpool_t *pool,void*(*function)(void* arg),void *arg)
{
    pthread_mutex_lock(&(pool->lock));

    while((pool->queue_size == pool->queue_max_size) && (!pool->shutdown))
    {
        pthread_cond_wait(&(pool->queue_not_full),&(pool->lock));
    }
    if(pool->shutdown)
    {
        pthread_mutex_unlock(&(pool->lock));   //线程池关闭了，为啥不退出？，反而解锁。
    }


    //清空工作线程调用的回调函数 的参数arg
    if(pool->task_queue[pool->queue_rear].arg!= NULL)
    {
        free(pool->task_queue[pool->queue_rear].arg);
        pool->task_queue[pool->queue_rear].arg = NULL;
    }

    //添加任务到任务队列中
    pool->task_queue[pool->queue_rear].function = function;
    pool->task_queue[pool->queue_rear].arg = arg;
    pool->queue_rear = (pool->queue_rear+1) % (pool->queue_max_size);  //队尾指针移动，模拟环形
    pool->queue_size++;

    //添加完任务以后，队列不为为空，唤醒线程池中的 等待处理任务的线程
    pthread_cond_signal(&(pool->queue_not_empty));   //唤醒至少一个线程.
    pthread_mutex_unlock(&(pool->lock));
    return 0;
}


void *process(void *arg)
{
    printf("pthread %lu work on task %d\n",pthread_self(),(int)arg);
    sleep(1);
    printf("task %d is end\n",(int)arg);

    return NULL;

}




int main(void)   //测试代码
{
    pthreadpool_t *thp = pthreadpool_create(3,100,100);  //最小线程数，最大线程数，队列的最大值。
    
    int num[20],i;
    for(int i=0;i<20;i++)
    {
        num[i] = i;
        printf("add task:%d\n",i);
        pthreadpool_add(thp,process,(void*)&num[i]);   //向任务队列中添加任务，自动被线程池接受处理。

    }
    sleep(10);
    pthreadpool_destroy(thp);
    return 0;
}
