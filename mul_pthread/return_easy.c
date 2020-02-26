#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


typedef struct mydata_s
{
    int a;
    char buf[100];
}exit_t;


void *foo(void *argc)
{
    exit_t *obj = (exit_t*)argc;
    obj->a = 100;
    strcpy(obj->buf,"jijijijijkanon");
    pthread_exit((void*)obj);


}
void *foo1(void *argc)
{
    struct mydata_s *ev = (struct mydata_s*)malloc(sizeof(struct mydata_s));
    ev->a = 100;
    strcpy(ev->buf,"wangyu");
    pthread_exit((void *)ev);

}

int main(void)
{
    //外部定义会产生乱码!
    pthread_t tid;
    exit_t *ev = (exit_t *)malloc(sizeof(exit_t));
    pthread_create(&tid,NULL,foo,(void*)ev);
    pthread_join(tid,(void**)&ev);
    printf("%s\n",ev->buf);







    //内部定义,
    /* struct mydata_s *get; */
    /* pthread_create(&tid,NULL,foo1,NULL); */

    /* pthread_join(tid,(void **)&get); */
    /* printf("%s\n",get->buf); */

    


}









