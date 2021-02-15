#ifndef __KACACHEPOOL_H__
#define __KACACHEPOOL_H__

#include <vector>
#include <string>
#include <map>
#include <list>
#include <iostream>

#include <mutex>
#include <condition_variable>

#include "../../Util/ConfigFileReader.h"
#include <hiredis/hiredis.h>

class KaCachePool;

class CacheConn
{
public:
    CacheConn(KaCachePool *pCachePool);
    ~CacheConn();
    int Init();
    const char *getPoolName();

    std::string get(std::string key);
    std::string setExpire(std::string key, int timeout, std::string value);
    //这里使用ref可以减少copy
    std::string set(std::string key, std::string &value);

    //批量获取
    bool mget(const std::vector<std::string> &keys, std::map<std::string, std::string> &ret_value);
    //判断一个key是否存在
    bool isExist(std::string &key);

    long incrBy(std::string key, long value);
    long incr(std::string key);
    long decr(std::string key);

    /*for hash operation*/
    /*
     *      
     *
     *      key    ----->           value     field_1   field_2
     *                                        value_1   value_2
     *
     * */
    /*注意返回值，失败返回0，成功返回1;
    *   对于字符串返回值，失败返回空字符串，成功返回有效字符串
    */
    long hdel(std::string key, std::string field);
    std::string hget(std::string key, std::string field);
    bool hgetAll(std::string key, std::map<std::string, std::string> &ret_value);
    long hset(std::string key, std::string field, std::string value);

    //为哈希表中的字段值加上指定的增量值，可以为负数，这样就是减法操作
    long hincrBy(std::string key, std::string field, long value);

    std::string hmset(std::string key, std::map<std::string, std::string> &hash);

    bool hmget(std::string key, std::list<std::string> &fields, std::list<std::string> &ret_value);

    /*
     * for list operator
     *  简单的字符串列表，按照插入顺序排序;
     *  
     *
     * */
    long lpush(std::string key, std::string value);
    long rpush(std::string key, std::string value);
    long llen(std::string key);
    bool lrange(std::string key, long start, long end, std::list<std::string> &ret_value);

    // TODO
    //完善list的删除操作，以及其他操作

    //TODO 更多的数据结构的完善

private:
    KaCachePool *m_pKcaChePool;
    redisContext *m_pContext;
    uint64_t m_last_connect_time;
};

class KaCachePool
{
public:
    explicit KaCachePool(const char *pool_name, const char *server_ip, uint16_t server_port, int db_num, int max_conn_cnt);
    ~KaCachePool();

    int Init();

    CacheConn *getCacheConn();
    void RelCacheConn(CacheConn *pCacheConn);

    const char *getPoolName()
    {
        return m_pool_name.c_str();
    }

    const char *getServerIP()
    {
        return m_server_ip.c_str();
    }

    int getServerPort()
    {
        return m_server_port;
    }

    int getDBNum()
    {
        return m_db_number;
    }

private:
    std::string m_pool_name;
    std::string m_server_ip;
    uint16_t m_server_port;
    int m_db_number; //在redis中默认有16个数据库，需要选取一个使用

    int m_cur_conn_cnt; //当前连接的数量
    int m_max_conn_cnt; //最大连接的数量
    std::list<CacheConn *> m_free_list;

    //保证free_list安全的锁成员数据
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

/*连接池管理器，全局唯一*/
class KaCacheManager
{
public:
    /*no copy*/

    KaCacheManager(const KaCacheManager &obj) = delete;
    const KaCacheManager &operator=(const KaCacheManager &obj) = delete;

    ~KaCacheManager() = default;

    static KaCacheManager *getInstance();

    int Init();
    CacheConn *getCacheConn(const char *pool_name);
    void RelCacheConn(CacheConn *pCacheConn);

private:
    KaCacheManager() = default;
    static KaCacheManager *m_pKaCaChePool_manager;
    std::map<std::string, KaCachePool*> m_pKaCaChePool_map;
};

#endif
