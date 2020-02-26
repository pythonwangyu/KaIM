### 关于线程的退出
- 1.使用exit
    一旦使用了exit就会退出整个进程，在线程的程序中，尽量不要使用
    exit，并且创建其他线程的主程序也是一个线程，常被称为主线程。
- 2，使用return
    在主线程中使用return会退出整个main函数，等价于退出线程。
    可以在其他线程中使用return，因为retrun的实质，是被调函数的返回。
- 3.使用pthread_exit(NULL);
    建议使用pthread_exit退出线程，这样不会关闭整个线程。

#### 函数原型pthread_exit(void *retval)
```c
The  pthread_exit() function terminates the calling thread and returns a value via retval that (if the
thread is joinable) is available to another thread in the same process that calls pthread_join(3).

void pthread_exit(void *retval);

```


### 线程之间参数的传递
- 1.在ptthread_create()函数中可以使用传递出

```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);

```
我们可以通过**范型指针**将数据传递进入**函数指针**中，以供线程使用。线程在结束的时候会留下一些信息，需要其他线程对这些信息
进行收集，使用函数pthread_join对指定的线程信息进行回收，这样就可以收集到一个线程传递回来的信息。
```c
int pthread_join(pthread_t thread, void **retval);

成功 0 ；

失败 errno;
```











