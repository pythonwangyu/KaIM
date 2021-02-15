## Redis客户端的编译
redis具体中文参考文档:https://www.redis.net.cn/tutorial/3501.html
#### 1、服务器上Redis的安装
- 系统，ubuntu18

```bash
wget https://download.redis.io/releases/redis-6.0.10.tar.gz
tar -zxvf redis-6.0.10.tar.gz
make 
sudo make install
默认安装到、/usr/local/bin 目录下：
```

- 默认启动redis会在终端下显示,设置conf文件使之daemon运行

```bash
#建立redis配置文件夹
sudo mkdir /etc/redis/
sudo cp redis.conf /etc/redis/6379.conf
#将6379.conf文件中的daemon 修改为yes
........
#执行
redis-server /etc/redis/6379.conf
#查看是否成功执行
ps -ef|grep redis
```

- 多线程IO启动

修改conf文件

```bash
/*6379.conf*/
#开启多线程io
io-threads-do-reads yes
#设定io线程数
io-threads 4
```

- 自己尝试抓包分析

```bash
sudo tcpdump -i any dst host 127.0.0.1 and port 6379 -XX
#加-XX 显示更加直观
```

#### 2、基本数据结构

##### 1、字典

Redis使用一个全局的哈希表来保存它所有的键值对。一个哈希表其实就是一个数组，数组的每一个元素称为一个哈希桶，每一个桶中保存了键值对数据。但是实际上数据并不是保存在该数组中数组中保存的是指向具体值的指针，这样一来即使值是一个集合也可以使用哈希桶中的指针找到。

哈希桶具体的数据结构

```C
typedef struct dictEntry{		//节点
 void* key;
 union{
     void* val;
     uint64_t u64;
     int64_t s64;
     double d;
 }v;
 struct dictEntry* next;
}dictEntry;
```

```C++
typedef struct dictht{			//哈希表
 dictEntry** table;				//指向指针数组的指针
 unsigned long size;
 unsigned long sizemask;
 unsigned long used;
}dictht;
```

```C++
typedef struct dict {			//字典
	dictType* type;				//类型特定函数，用于处理键和值
	void* privdate;				//type中的函数的传入参数
	dictht ht[2];				//哈希表
	long rehashidx;				//rehash索引,如果为-1,则说明没有rehash
	unsigned long iterators;	//当前正在运行的迭代器
}dict;
```

```C
typedef struct dictType {
	uint64_t(*hasFunction)(const void* key);					//计算哈希值的函数
	void* (*keyDup)(void* privdata, const void* key);			//复制键的函数
	void* (*valdup)(void* privdata, const void* obj);			//复制值的函数
	void (*keyDestructor)(void* privdata, void* key);			//销毁键的函数
	void (*valDestructor)(void* privdata, void* obj);			//销毁值的函数
	int (*keyCompare)(void* privdata, const void* key1, const void* key2);	
}dictType;
```

- 在O(1)时间内查找到键值对的过程

```c++
//1、根据键key和哈希函数，求取出哈希值
hash = dict->type->hashFunction(key);
//2、根据哈希值和sizemask计算出index。
index = hash & dict->ht[x].sizemask;
//3、根据index计算出具体的哈希桶
dict->ht[x]->table+index;  //这个就是具体的dictEntry的指针
```

- 存在的一些问题：

（1）哈希表冲突：两个key对应同一个哈希桶。

Redis解决方法：拉链法。在entry中加入一个next指针指向具有相同的哈希值的新的entry。

但是随着越来越多的数据的插入可能会导致链表的长度越来越长。这样就会到导致查找的时间边长，影响响应的速度。因此需要引进一个新的方法来降低链表的长度。那就是rehash。Redis的rehash就是增加现有的哈希桶（也就是entry）的数量，让逐渐多的entry元素尽可能均匀分散的哈希表中。

Redis在设计中，使用一个字典来存储所有的键值。在字典的数据结构中，存在着两个哈希表。dictht ht[2];默认开始使用哈希表1存储数据，此时哈希表2没有分配内存空间。当数据开始增多达到rehash的条件，就会发生rehash！

```bash
1.给哈希表2分配更大的空间，哈希表1的两倍空间
2.把哈希表1的数据拷贝到哈希表2中
3.释放哈希表1的内存空间
```

这里又引入了一个问题：在步骤2的大量数据拷贝过程中，可能会导致Redis的mainthread的阻塞，无法处理其他的请求，造成响应过慢的问题。

因此引入了渐进式哈希的方法优化拷贝过程，其实质就是将一个大量拷贝，分散到多次loop过程中。即不是一次将哈希表一次拷贝完毕，而是多次的进行。

##### 2、链表

链表节点

```c
typedef struct listNode{
    struct listNode* prev;
    struct listNode* next;
    void *value;
}listNode;
```

链表

```C
typedef struct list{
    listNode* head;
    listNode* tail;
    unsigned long len;
    void* (*dup)(void* ptr);
   	void* free(void* ptr);
    int (*match)(void* ptr,void* key);
}list;
```



















