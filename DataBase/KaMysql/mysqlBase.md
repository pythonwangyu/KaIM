## 函数说明

更加详细的说明：http://www.mysqlab.net/docs/view/refman-5.1-zh/chapter/apis.html

### 返回值说明

返回值的函数通常会返回指针或者整数，返回指针的函数将返回非NULL,表示成功；返回NULL表示失败。返回整数的函数返回零表示成功；返回非零表示失败。

当发生错误的时候，可以调用指定函数进行错误检测。

```c
unsigned int mysql_errno(MYSQL *mysql);
//返回最近调用API的错误代码，0返回值表示未出现错误。错误码在errmsg.h头文件中
//如果成功，所有向服务器请求信息的函数均会复位mysql_errno()

const char *mysql_error(MYSQL *mysql);
//返回最近调用API的错误信息字符串
```

#### 1、初始化函数

- 分配或者初始化与mysql_real_connect函数相适应的MYSQL对象，若传入空指针，会分配新的初始化对象，返回分配的地址，如果分配了新对象，必须使用mysql_close函数关闭。

```C
MYSQL *mysql_init(MYSQL *mysql);
//成功返回执行MYSQL对象的指针，失败返回NULL。
```

- 测试连接

```C
int mysql_ping(MYSQL* mysql);
//成功返回0，失败返回非零；返回非零也不意味着服务器本身关闭
```

- 设置连接选项:应在mysql_init函数之后、以及mysql_connect函数或mysql_real_connect函数之前调用mysql_options函数

```C
int mysql_options(MYSQL *mysql, enum mysql_option option, const char *arg);
//成功时返回0。如果使用了未知选项，返回非0值
```

- 实际连接

```C
MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag);
//如果连接成功，返回MYSQL*连接句柄。如果连接失败，返回NULL。对于成功的连接，返回值与第1个参数的值相同
```

 “host”的值必须是主机名或IP地址。如果“host”是NULL或字符串"localhost"，连接将被视为与本地主机的连接。如果操作系统支持套接字（Unix）或命名管道（Windows），将使用它们而不是TCP/IP连接到服务器。

如果unix_socket不是NULL，该字符串描述了应使用的套接字或命名管道。注意，“host”参数决定了连接的类型。

client_flag的值通常为0，但是，也能将其设置为下述标志的组合，以允许特定功能：

- 关闭

关闭前面打开的连接。如果句柄是由mysql_init()或mysql_connect()自动分配的，mysql_close()还将解除分配由mysql指向的连接句柄。

```C
void mysql_close(MYSQL *mysql)
```

#### 2、字符集相关函数

- 返回当前字符集

```C
const char *mysql_character_set_name(MYSQL *mysql);
//
```

- 设置字符集

```C
int mysql_set_character_set(MYSQL *mysql, char *csname);
mysql_set_charset_name(&mysql, "utf8");
```

#### 3、查询

- 非二进制安全

```C
int mysql_query(MYSQL *mysql, const char *query);
//成功返回0，失败返回非零
```

执行由“Null终结的字符串”查询指向的SQL查询。正常情况下，字符串必须包含1条SQL语句，而且不应为语句添加终结分号（‘;’）或“\g”。

正常情况下只会执行第一个语句，想要执行多条语句需要在connect函数中设置或者在

```C
mysql_set_server_option(MYSQL_OPTION_MULTI_STATEMENTS_ON);
CLIENT_MULTI_STATEMENTS选项指定给mysql_real_connect();
```

- 二进制安全(因为有len)

```c
int mysql_real_query(MYSQL *mysql, const char *query, unsigned long length);
//如果查询成功，返回0。如果出现错误，返回非0值。
```

#### 4、查询结果

对于成功检索数据的每个查询（SELECT、SHOW、DESCRIBE、EXPLAIN），必须调用mysql_store_result函数或mysql_use_result函数。

```C
（1）一次性读取完毕
MYSQL_RES *mysql_store_result(MYSQL *mysql);
//mysql_store_result()将查询的全部结果读取到客户端，分配1个MYSQL_RES结构，并将结果置于该结构中。
（2）多次读取
MYSQL_RES *mysql_use_result(MYSQL *mysql)
//不将数据全部提取到客户端，而是必须调用mysql_fetch_row()函数每一行检索，但是这会绑定服务器，并阻止其他线程更新任何表
//不应与从mysql_use_result()返回的结果一起使用mysql_data_seek()、mysql_row_seek()、mysql_row_tell()、mysql_num_rows()或mysql_affected_rows()，也不应发出其他查询，直至mysql_use_result()完成为止。（但是，提取了所有行后，mysql_num_rows()将准确返回提取的行数)。一旦完成了对结果集的操作，必须调用mysql_free_result()。
```

如果查询未返回结果集，mysql_store_result函数将返回NULL指针（例如，如果查询是INSERT语句）。如果读取结果集失败，mysql_store_result函数还会返回NULL指针。通过检查mysql_error()是否返回非空字符串，mysql_errno()是否返回非0值，或mysql_field_count()是否返回0，可以检查是否出现了错误。

对于其他查询，不需要调用mysql_store_result函数或mysql_use_result函数，但是如果在任何情况下均调用了mysql_store_result函数，它也不会导致任何伤害或性能降低。通过检查mysql_store_result()是否返回NULL，可检测查询是否没有结果集（以后会更多）。

- 对于结果，成功返回MYSQL_RES结构体，失败返回NULL。

#### 5、查询结果的长度

```
| fieldName_1 | fieldName_2 | fieldName_3| ... | fieldName_n |
|   value_1   |  value_2    |  value_3   | ... |  value_n    |
|   ........................................................ |
|   value_1   |  value_2    |  value_3   | ... |  value_n    |
```

- 返回结果集中的行数（需要MYSQL_RES传入）

```c
my_ulonglong mysql_num_rows(MYSQL_RES *result);
```

- 返回结果集中的字段数

```c
unsigned int mysql_field_count(MYSQL *mysql);
```

- 返回结果集中的字段数（需要MYSQL_RES传入）与mysql_field_count()函数

```c
unsigned int mysql_num_fields(MYSQL_RES *result);
```

#### 6、Fetch函数族

- mysql_fetch_field函数

返回当前列的MYSQL_FIELD结构。如果未剩余任何列，返回NULL。

```c
MYSQL_FIELD *mysql_fetch_field(MYSQL_RES *result);
//example
MYSQL_FIELD* field;
while((field = mysql_fetch_field(result)))
{
    printf("field name %s \n",field->name);
}
```

- mysql_fetch_fields函数

对于结果集，返回所有MYSQL_FIELD结构的数组。每个结构提供了结果集中1列的字段定义。

```c
MYSQL_RES *mysql_fetch_fields(MYSQL_RES* result);
unsigned int num_fields;
unsigned int i;
MYSQL_FIELD *fields;
num_fields = mysql_num_field(result);
fields = mysql_fetch_fields(result);
for(int i = 0; i < fields; ++i)
{
    printf("Field %u is %s \n",i,fields[i].name);
}
```

- mysql_fetch_lengths函数

无符号长整数的数组表示各列的大小（不包括任何终结NULL字符）。如果出现错误，返回NULL。这里返回的是每一个field的字符串的长度。

```c
unsigned long *mysql_fetch_lengths(MYSQL_RES *result)
```

- mysql_fetch_row函数

下一行的MYSQL_ROW结构。如果没有更多要检索的行或出现了错误，返回NULL。

```c
MYSQL_ROW mysql_fetch_row(MYSQL_RES *result);
```

行内值的数目由mysql_num_fields(result)给出。如果行中保存了调用mysql_fetch_row()返回的值，将按照row[0]到row[mysql_num_fields(result)-1]，访问这些值的指针。

example：

```c
MYSQL_ROW row;
while((row = mysql_fetch_row(result)))
{
    for(int i = 0; i < fieldNums; ++i)
    {
        cout << row[i] << endl;			//row实际上是char** 类型，row[0] char*类型
    }
}
```

#### 7、释放结果集的内存

释放由mysql_store_result()、mysql_use_result()、mysql_list_dbs()等为结果集分配的内存。完成对结果集的操作后，必须调用mysql_free_result()释放结果集使用的内存。释放完成后，不要尝试访问结果集。

```c
void mysql_free_result(MYSQL_RES* result);
```

#### 8、链接mysqlclient

```
g++ main.cpp -lmysqlclient
在Clion中插入:
link_libraries(mysqlclient)			//链接第三方库
link_libraries(pthread)
add_executable(Test main.cpp)
```

#### 9、example

```C++
/*mian.cpp*/
#include <iostream>
#include <mysql/mysql.h>
using namespace std;

int main() {

    MYSQL* m_mysql;
    m_mysql = mysql_init(NULL);     //这里需要注意,这里为NULL.
    if(!m_mysql)
    {
        cout << "mysql_init() error" << endl;
        return -1;
    }
    mysql_real_connect(m_mysql,"127.0.0.1","kanon","python2018",
                       "KaIM",0,NULL,0);
    //查询语句
    int ret  = mysql_query(m_mysql,"desc t_chatmsg");
    if(ret)
    {
        cout << "mysql_query() error" << endl;
        return -1;
    }
    else
    {
        cout << "查询成功" << endl;
    }
	//存储查询结果
    MYSQL_RES *res = mysql_store_result(m_mysql);           //查询存储
    if(!res)
    {
        cout << "mysql_store_result() error" << endl;
        return -1;
    }

    unsigned int fieldCount = mysql_num_fields(res);
    unsigned int rowCount = mysql_num_rows(res);

    cout << "fieldCount = " << fieldCount << endl;
    cout << "rowCount = " << rowCount << endl;

    MYSQL_FIELD * fields = mysql_fetch_fields(res);
    for(int i = 0; i < fieldCount; ++i)
    {
        cout << i << endl;
        cout << "fields[i].name " << fields[i].name << endl;
        cout << "fields[i].type " << fields[i].type  << endl;
        cout << "fields[i].length " << fields[i].length << endl;
    }

    MYSQL_ROW row;
    int j = 0;
    std::string str;
    while((row = mysql_fetch_row(res)))     //为什么有些row显示不全？
    {
        cout << "j = " << j << endl;
        for(int i = 0;i < fieldCount; ++i)
        {
            cout << row[i] << endl;
        }
        ++j;
    }
    mysql_free_result(res);
    mysql_close(m_mysql);
    return 0;
}
```
