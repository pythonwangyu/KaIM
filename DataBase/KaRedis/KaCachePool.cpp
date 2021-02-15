#include "KaCachePool.h"

#include <string.h>

#define MIN_CACHE_CONN_CNT 2


/*
*       for Cache basic connection
*/

CacheConn::CacheConn(KaCachePool *pCachePool)
{
    m_pContext = NULL;
    m_pKcaChePool = pCachePool;
    m_last_connect_time = 0;
}

CacheConn::~CacheConn()
{
    if (m_pContext)
    {
        redisFree(m_pContext);
        m_pContext = NULL;
    }
}

int CacheConn::Init()
{
    if (m_pContext)
    {
        return 0;
    }
    uint64_t cur_time = (uint64_t)time(NULL);
    if (cur_time < m_last_connect_time + 4)
    {
        return 1;
    }
    m_last_connect_time = cur_time;
    //在指定时间内连接上去
    struct timeval timeout = {0, 200000};
    m_pContext = redisConnectWithTimeout(m_pKcaChePool->getServerIP(),
                                         m_pKcaChePool->getServerPort(), timeout);
    if (!m_pContext || m_pContext->err)
    {
        if (m_pContext)
        {
            printf("redisconnect failed: %s\n", m_pContext->errstr);
            redisFree(m_pContext);
            m_pContext = NULL;
        }
        else
        {
            printf("redisConnect failed\n");
        }
        return 1;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "select %d", m_pKcaChePool->getDBNum());
    if (reply && (reply->type == REDIS_REPLY_STATUS) && (strncmp(reply->str, "OK", 2) == 0))
    {
        freeReplyObject(reply);
        return 0;
    }
    else
    {
        printf("select dbnum error\n");
        return 2;
    }
}

std::string CacheConn::get(std::string key)
{
    std::string value;
    if (Init())
    {
        return value;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "GET %s", key.c_str());
    if (!reply)
    {
        printf("redisCommand failed: %s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return value;
    }
    if (reply->type == REDIS_REPLY_STRING)
    {
        value.append(reply->str, reply->len);
    }
    freeReplyObject(reply);
    return value;
}

std::string CacheConn::setExpire(std::string key, int timeout, std::string value)
{
    std::string ret_value;
    if (Init())
    {
        return ret_value;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "SETEX %s %d %s", key.c_str(), timeout, value.c_str());
    if (!reply)
    {
        printf("redisCommamd failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return ret_value;
    }
    ret_value.append(reply->str, reply->len);
    freeReplyObject(reply);
    return ret_value;
}

std::string CacheConn::set(std::string key, std::string &value)
{
    std::string ret_value;
    if (Init())
    {
        return ret_value;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "SET %s %s", key.c_str(), value.c_str());
    if (!reply)
    {
        printf("redisCommand failed%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return ret_value;
    }
    ret_value.append(reply->str, reply->len);
    freeReplyObject(reply);
    return ret_value;
}

bool CacheConn::mget(const std::vector<std::string> &keys, std::map<std::string, std::string> &ret_value)
{
    if (Init())
    {
        return false;
    }
    if (keys.empty())
    {
        return false;
    }
    std::string strkey;
    bool bFirst = true;
    for (auto it = keys.begin(); it != keys.end(); ++it)
    {
        if (bFirst)
        {
            bFirst = false;
            strkey = *it;
        }
        else
        {
            strkey += " " + *it;
        }
    }
    if (strkey.empty())
    {
        return false;
    }
    strkey = "MGET" + strkey;
    redisReply *reply = (redisReply *)redisCommand(m_pContext, strkey.c_str());
    if (!reply)
    {
        printf("redisCommand failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return false;
    }
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (size_t i = 0; i < reply->elements; ++i)
        {
            redisReply *Child_reply = reply->element[i];
            if (Child_reply->type == REDIS_REPLY_STRING)
            {
                ret_value[keys[i]] = Child_reply->str;
            }
        }
    }
    freeReplyObject(reply);
    return true;
}

bool CacheConn::isExist(std::string &key)
{
    if (Init())
    {
        return false;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "EXISTS %s", key.c_str());
    if (!reply)
    {
        printf("redisCommand failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        return false;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    if (ret_value == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

long CacheConn::incrBy(std::string key, long value)
{
    if (Init())
    {
        return -1;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "INCRBY %s %ld", key.c_str(), value);
    if (!reply)
    {
        printf("redisCommand failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}
long CacheConn::incr(std::string key)
{
    if (Init())
    {
        return -1;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "INCR %s", key.c_str());
    if (!reply)
    {
        printf("redisCommand failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}

long CacheConn::decr(std::string key)
{
    if (Init())
    {
        return -1;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "DECR %s", key.c_str());
    if (!reply)
    {
        printf("redisCommand failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}

/*****************************************************************************************************************/
//for hash_map operation
long CacheConn::hdel(std::string key, std::string field)
{
    if (Init())
    {
        return 0;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "HDEL %s %s", key.c_str(), field.c_str());
    if (!reply)
    {
        printf("redisCommand failed:%s", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return 0;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}

std::string CacheConn::hget(std::string key, std::string field)
{
    std::string ret_value;
    if (Init())
    {
        return ret_value;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "GET %s %s", key.c_str(), field.c_str());
    if (!reply)
    {
        printf("redisCommand failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return ret_value;
    }
    ret_value.append(reply->str, reply->len);
    freeReplyObject(reply);
    return ret_value;
}

bool CacheConn::hgetAll(std::string key, std::map<std::string, std::string> &ret_value)
{
    if (Init())
    {
        return false;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "HEGTALL %s ", key.c_str());
    if (!reply)
    {
        printf("redisCommand:%s\n", key.c_str());
        redisFree(m_pContext);
        m_pContext = NULL;
        return false;
    }
    if ((reply->type == REDIS_REPLY_ARRAY) && (reply->elements % 2 == 0))
    {
        for (size_t i = 0; i < reply->elements; i += 2)
        {
            redisReply *fieldReply = reply->element[i];
            redisReply *valueReply = reply->element[i + 1];

            std::string field(fieldReply->str, fieldReply->len);
            std::string value(valueReply->str, fieldReply->len);
            ret_value.insert(std::make_pair(field, value));
        }
    }
    freeReplyObject(reply);
    return true;
}

long CacheConn::hset(std::string key, std::string field, std::string value)
{
    if (Init())
    {
        return -1;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
    if (!reply)
    {
        printf("redisCommand failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret = reply->integer;
    freeReplyObject(reply);
    return ret;
}

long CacheConn::hincrBy(std::string key, std::string field, long value)
{
    if (Init())
    {
        return -1;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "HINCRBY %s %s %ld", key.c_str(), field.c_str(), value);
    if (!reply)
    {
        printf("reidsCommand failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}

/*if sucess ,it will return OK*/
std::string CacheConn::hmset(std::string key, std::map<std::string, std::string> &hash)
{
    std::string ret_value;
    if (Init())
    {
        return ret_value;
    }
    int argc = hash.size() * 2 + 2;
    const char **argv = new const char *[argc]; //指向指针数组的指针
    if (!argv)
    {
        return ret_value;
    }
    argv[0] = "HMSET";
    argv[1] = key.c_str();
    int i = 2;
    for (auto it = hash.begin(); it != hash.end(); ++it)
    {
        argv[i++] = it->first.c_str();
        argv[i++] = it->second.c_str();
    }
    redisReply *reply = (redisReply *)redisCommandArgv(m_pContext, argc, argv, NULL); //care this is redisCommandArgv;
    if (!reply)
    {
        printf("redisCommandArgv failed:%s", m_pContext->errstr);
        delete[] argv; //do‘t forget  release the stack memory
        redisFree(m_pContext);
        m_pContext = NULL;
        return ret_value;
    }
    ret_value.append(reply->str, reply->len);
    delete[] argv;
    freeReplyObject(reply);
    return ret_value;
}

bool CacheConn::hmget(std::string key, std::list<std::string> &fields, std::list<std::string> &ret_value)
{
    if (Init())
    {
        return false;
    }
    int argc = fields.size() + 2;
    const char **argv = new const char *[argc];
    argv[0] = "HMGET";
    argv[1] = key.c_str();
    int i = 2;
    for (auto it = fields.begin(); it != fields.end(); ++it)
    {
        argv[i++] = it->c_str();
    }
    redisReply *reply = (redisReply *)redisCommandArgv(m_pContext, argc, argv, NULL);
    if (!reply)
    {
        printf("redisCommandargc failed:%s\n", m_pContext->errstr);
        delete[] argv;
        redisFree(m_pContext);
        m_pContext = NULL;
        return false;
    }
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (size_t i = 0; i < reply->elements; ++i)
        {
            redisReply *value_reply = reply->element[i];
            std::string value(value_reply->str, value_reply->len);
            ret_value.push_back(value);
        }
    }
    delete[] argv;
    freeReplyObject(reply);
    return true;
}

long CacheConn::lpush(std::string key, std::string value)
{
    if (Init())
    {
        return -1;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "LPUSH %s %s", key.c_str(), value.c_str());
    if (!reply)
    {
        printf("redisCommand failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}

long CacheConn::rpush(std::string key, std::string value)
{
    if (Init())
    {
        return -1;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "RPUSH %s %s", key.c_str(), value.c_str());
    if (!reply)
    {
        printf("redisCommand failed:%s\n", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}

long CacheConn::llen(std::string key)
{
    if (Init())
    {
        return -1;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "LLEN %s", key.c_str());
    if (!reply)
    {
        printf("redisCommand failed:%s", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}

bool CacheConn::lrange(std::string key, long start, long end, std::list<std::string> &ret_value)
{
    if (Init())
    {
        return false;
    }
    redisReply *reply = (redisReply *)redisCommand(m_pContext, "LRANGE %s %d %d", key.c_str(), start, end);
    if (!reply)
    {
        printf("redisCommand failed:%s", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return false;
    }
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (size_t i = 0; i < reply->elements; ++i)
        {
            redisReply *value_reply = reply->element[i];
            std::string value(value_reply->str, value_reply->len);
            ret_value.push_back(value);
        }
    }
    freeReplyObject(reply);
    return true;
}

/******************************************************************************************************/
//for CaChePool operator

/**
 * 
 *          
 *  list<CaCheConn*>            管理空闲的连接链表
 * 
 * 
 * 
 * 
*/

KaCachePool::KaCachePool(const char *pool_name, const char *server_ip, uint16_t server_port, int db_num, int max_conn_cnt)
{
    m_pool_name = pool_name;
    m_server_ip = server_ip;
    m_server_port = server_port;
    m_db_number = db_num;
    m_max_conn_cnt = max_conn_cnt;
    m_cur_conn_cnt = MIN_CACHE_CONN_CNT;
}

KaCachePool::~KaCachePool()
{
    std::unique_lock<std::mutex> guard(m_mutex);
    for (auto it = m_free_list.begin(); it != m_free_list.end(); ++it)
    {
        CacheConn *pConn = *it;
        delete pConn;
        pConn = NULL;
    }
    m_free_list.clear();
    m_cur_conn_cnt = 0;
}

int KaCachePool::Init()
{
    for (int i = 0; i < m_cur_conn_cnt; ++i)
    {
        CacheConn *pConn = new CacheConn(this);
        if (pConn->Init())
        {
            delete pConn;
            return 1;
        }
        m_free_list.push_back(pConn);
    }
    return 0;
}

CacheConn *KaCachePool::getCacheConn()
{
    std::unique_lock<std::mutex> guard(m_mutex);
    while (m_free_list.empty())
    {
        if (m_cur_conn_cnt >= m_max_conn_cnt)
        {
            m_cond.wait(guard);
        }
        else
        {
            CacheConn *pConn = new CacheConn(this);
            int ret = pConn->Init();
            if (ret)
            {
                printf("Init CacheConn failed\n");
                delete pConn;
                return NULL;
            }
            else
            {
                m_free_list.push_back(pConn);
                m_cur_conn_cnt++;
            }
        }
    }
    CacheConn *pConn = m_free_list.front();
    m_free_list.pop_front();
    return pConn;
}

void KaCachePool::RelCacheConn(CacheConn *pCacheConn)
{
    std::unique_lock<std::mutex> guard(m_mutex);
    auto it = m_free_list.begin();
    for (; it != m_free_list.end(); ++it)
    {
        if (*it == pCacheConn)
        {
            break;
        }
    }
    if (it == m_free_list.end())
    {
        m_free_list.push_back(pCacheConn);
    }
    m_cond.notify_all();
}

/**
 * 
 *      KaDBManager 管理器
 * 
 *   static KaCacheManager *m_pKaCaChePool_manager;
 * 
 * 
 * 
*/

//用于初始化静态成员用于单例实现
KaCacheManager *KaCacheManager::m_pKaCaChePool_manager = NULL;


KaCacheManager* KaCacheManager::getInstance(){
    if(!m_pKaCaChePool_manager)
    {
        m_pKaCaChePool_manager = new KaCacheManager();
        if(m_pKaCaChePool_manager->Init())
        {
            delete m_pKaCaChePool_manager;
            m_pKaCaChePool_manager = NULL;
        }
    }
    return m_pKaCaChePool_manager;
}
    


int KaCacheManager::Init()
{
    ConfigFileReader reader("../DataConf/redis.conf");
    //创建若干个指定的缓存连接池
    //这里假设只能读取一个
    const char* ip = reader.GetConfigName("KaIM_host");
    const char* port = reader.GetConfigName("KaIM_port");
    const char* dbNum = reader.GetConfigName("KaIM_DBNUM");
    const char* max_conn = reader.GetConfigName("KaIM_MAX_CONN");
    const char* Pool_name = "KaRedis_1";

    KaCachePool* pCachePool = new KaCachePool(Pool_name,ip,atoi(port),atoi(dbNum),atoi(max_conn));
    if(pCachePool->Init())
    {
        printf("Init Cache Pool failed\n");
        return 3;
    }   
    m_pKaCaChePool_map.insert(std::make_pair(Pool_name,pCachePool));
    return 0;
}

CacheConn* KaCacheManager::getCacheConn(const char* pool_name)
{
    auto it = m_pKaCaChePool_map.find(pool_name);
    if(it != m_pKaCaChePool_map.end())
    {
        return it->second->getCacheConn();
    }
    else
    {
        return NULL;
    }
}


void KaCacheManager::RelCacheConn(CacheConn* pCacheConn)
{
    if(!pCacheConn)
    {
        return;
    }
    auto it = m_pKaCaChePool_map.find(pCacheConn->getPoolName());
    if(it != m_pKaCaChePool_map.end())
    {
        return it->second->RelCacheConn(pCacheConn);
    }
}
