#### 1、mysql_init
```C
MYSQL* mysql_init(MYSQL* mysql);
分配、初始化一个MYSQL对象。
返回值：
    成功返回MYSQL的指针地址
    失败返回NULL
```

#### 2、mysql_close

关闭前面打开的连接，如果句柄是由 mysql_init() 或者 mysql_connect() 自动分配的，mysql_close() 还将解除分配由 mysql 指向的连接句柄。

```C++
void mysql_close(MYSQL* mysql);
```

#### 3、mysql_real_connect

mysql_real_connect() 尝试与运行在主机上的MYSQL 数据库引擎建立连接

```C
MYSQL* mysql_real_connect(MYSQL* mysql,const char* host,
                         const char* user,const char* passwd,
                         const char* db,const char* unix_socket,
                         long client_flag);
第一个参数：必须使用mysql_init()函数初始初始化。
第二个参数如果为NULL或者localhost，将与本地主机连接。
返回值：
    失败返回NULL
    成功返回与第一个参数相同的指针
```

#### 4、mysql_errno

```C++
unsigned int mysql_errno(MYSQL* mysql);
//返回最近调用API的错误代码，0表示未出现错误
```

#### 5、mysql_query

```C
int mysql_query(MYSQL* mysql,const char* query);
//执行由“NULL 终结的字符串”查询指向的SQL查询。正常情况下，字符串必须包含一条SQL语句，而且不应该为语句添加分号“;”或者“\g”。
//mysql_query() 不能用于包含二进制数据的查询，应使用mysql_real_query() 取而代之，因为二进制数据可能包含字符“\0”，mysql_query() 会将该字符解释为查询字符串结束。
返回值：
    成功放回0
    失败返回非零值
```

#### 6、mysql_real_query

```C
int mysql_real_query(MYSQL* mysql,const char* query,unsigned long length);
//成功：返回0；失败：非零值
//可以包含二进制数据的插叙
```

#### 7、mysql_store_result() / mysql_use_result()

返回MYSQL_RES变量获取查询结果数据。

```c
MYSQL_RES* mysql_store_result(MYSQL* mysql);
MYSQL_RES* mysql_use_result(MYSQL* mysql);
```

- 调用mysql_store_result()函数将从mysql服务器中查询所有数据都存储到客户端中，然后读取
- 调用mysql_use_result()函数初始化检索，以便后面一行一行读取结果集，它本身并没有从服务器读取任何数据，这种方式速度更快，并且所需内存更小。但是她会绑定服务器，阻止线程更新任何表，必须重复使用mysql_fetch_row读取数据，直到返回NULL，如果读取不干净会在下一次查询作为一部分返回。

#### 8、mysql_fetch_row

```C
MYSQL_ROW mysql_fetch_row(MYSQL_RES* result);
//那么result就是前面的mysq_store_result()函数返回的MYSQL_RES指针
```



