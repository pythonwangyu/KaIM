#ifndef __KADBMANAGER_H__
#define __KADBMANAGER_H__

#include "MysqlBase.h"

#include <mysql/mysql.h>
#include <mysql/errmsg.h>

#include <list>
#include <mutex>
#include <condition_variable>
#include <map>

#define MAX_QUERY_LEN 1024			//最大的查询长度


//前置声明
class KaDBPool;

class KaDBConn{
public:
    explicit KaDBConn(KaDBPool* dbPool);        //传入连接池指针，以获取连接池信息
    ~KaDBConn() = default;
    
    //初始化函数，实际创建连接
    int Init();
    
    //核心查询语句，将sql语句传入进行查询，目前仅支持单语句的查询。
    QueryResult* query(const char* sql);
    QueryResult* query(const std::string& sql)
    {
        return query(sql.c_str());
    }
    
    //格式化传入查询
    QueryResult* pquery(const char* format,...);

    bool execute(const char* sql);
    
    //返回插入id
    uint32_t getInsertId();

    //清除mysql_store在客户端的内存
    void clearStoreResults();

    //
    int32_t escapeString(char* szDst,const char* szSrc,uint32_t uSize);

    const char* getPoolName();      //获取该KaDBConn所属线程池的名字

    MYSQL* getMysql()
    {
        return m_mysql;
    }

private:
    KaDBPool*           m_DBPool;           //该连接所属的连接池
    MYSQL*              m_mysql;            //连接池

};

/******************************************基础连接信息数据结构**********************************************/
struct DatabaseInfo{        //数据库连接信息
    std::string strHost;        			//ip
    uint16_t    Port;           
    std::string strUser;
    std::string strPwd;
    std::string strDBName;
};

/**********************************************************************************************************/

/*
*			连接池类 KaDBPool
*			
*/

class KaDBPool{
public:
    explicit KaDBPool(const char* poolName, const char* db_server_ip, 
                      uint16_t db_server_port, const char* userName,
                      const char* passwd, const char *dbName, int maxConn);

    ~KaDBPool();
    
    int init();     //根据当前的设定的连接池数量，在连接池中创建指定数量的连接数量，交给链表管理


/********************************************************************************************/
    /*核心函数，获取一个空闲的连接，释放一个连接？*/        /*这两个函数是用于管理连接池的*/
    KaDBConn* getKaConn();

    void RelKaConn(KaDBConn* pConn);
/*******************************************************************************************/

    /*获取连接池属性的API*/
    const char* getPoolName(){
        return m_PoolName.c_str();
    }

    const char* getDBServerIP() const {
        return m_DatabaseInfo.strHost.c_str();
    }

    uint16_t getDBServerPort() const{
        return m_DatabaseInfo.Port;
    }

    const char* getUserName(){
        return m_DatabaseInfo.strUser.c_str();
    }

    const char* getPasswd(){
        return m_DatabaseInfo.strPwd.c_str();
    }

    const char* getDBName(){
        return m_DatabaseInfo.strDBName.c_str();
    }

private:
    std::string                     m_PoolName;             //连接池的名字
    DatabaseInfo                    m_DatabaseInfo;         //连接的信息
    int                             m_db_cur_conn_cnt;      //当前连接的数量
    int                             m_db_max_conn_cnt;      //最大的连接数量
    std::list<KaDBConn*>            m_free_list;            //list管理的连接池，已经连接的CONN
    std::mutex                      m_mutex;                //互斥锁
    std::condition_variable         m_cond;                 //条件变量
};



/*
 *
 *  连接池管理器，可以从指定的连接池中获取连接
 *  使用map管理连接池对象，采用单例模式全局唯一
 *
 *  TODO: use smart pointer monoter the KaDBConn
 * */
class KaDBManager{
public:
    KaDBManager(const KaDBManager& obj) = delete ;

    KaDBManager& operator=(const KaDBManager& obj) = delete ;

    static KaDBManager* getInstance();

    int init();

    //从指定的名字的连接池中取出一个连接

    KaDBConn* getDBConn(const char* dbPoolName);

    KaDBConn* getDBConn(std::string& dbPoolName);


    //释放连接池的函数
    void RelDBConn(KaDBConn* pConn);

private:
    KaDBManager() = default;
    static KaDBManager*             g_manager;          //静态成员变量，为了实现单例模式而设置
    std::map<std::string,KaDBPool*> m_DBPools;          //根据名字管理线程池，map数据结构
};



#endif //__KADBMANAGER_H__


