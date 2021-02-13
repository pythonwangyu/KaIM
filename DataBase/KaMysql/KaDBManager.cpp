#include "KaDBManager.h"
#include "../../Util/ConfigFileReader.h" 

//C lib
#include <cstdarg>
#include <cstring>


#define MIN_CONN_CNT 2 			//设定连接池的最小连接数


/*基础连接*/
KaDBConn::KaDBConn(KaDBPool* dbPool)
{
    m_DBPool = dbPool;
    m_mysql = NULL;
}


int KaDBConn::Init()
{
    m_mysql = mysql_init(NULL);         //初始化连接
    if(!m_mysql)
    {
        return -1;
    }
    bool reconnect = true;
    //设置连接选项
    mysql_options(m_mysql,MYSQL_OPT_RECONNECT,&reconnect);
    mysql_options(m_mysql,MYSQL_SET_CHARSET_NAME,"utf8mb4");
    
    //实际连接过程
    if(!mysql_real_connect(m_mysql,m_DBPool->getDBServerIP(),
                           m_DBPool->getUserName(),
                           m_DBPool->getPasswd(),
                           m_DBPool->getDBName(),
                           m_DBPool->getDBServerPort(),
                           NULL,0))
    {
        return -2;
    }
    return 0;
}


/*核心查询语句*/

/*返回NULL ---->error*/

QueryResult * KaDBConn::query(const char *sql) {
    if(!m_mysql)
    {
        if(0 != Init())
        {
            return NULL;
        }
    }
    if(!m_mysql)
        return NULL;
    MYSQL_RES *result = NULL;
    uint64_t rowCount = 0;
    uint32_t fieldCount = 0;

    int Ret = mysql_real_query(m_mysql,sql,strlen(sql));
    if(Ret)
    {
        unsigned int Error = mysql_errno(m_mysql);
        if(CR_SERVER_GONE_ERROR == Error)
        {
            if(0 != Init())
            {
                return NULL;
            }
            Ret = mysql_real_query(m_mysql,sql,strlen(sql));
            if(Ret)
            {
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
    }
    result = mysql_store_result(m_mysql);
    rowCount = mysql_affected_rows(m_mysql);
    fieldCount = mysql_field_count(m_mysql);
    if(!result)
    {
        return NULL;
    }
    //在堆中创建返回值，这个什么时候释放？
    QueryResult* queryResult = new QueryResult(result,rowCount,fieldCount);
    return queryResult;
}



QueryResult * KaDBConn::pquery(const char *format, ...) {
    if(!format)
        return NULL;
    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap,format);
    int res = vsnprintf(szQuery,MAX_QUERY_LEN,format,ap);		//这个函数用于拼接字符串，用途广泛
    va_end(ap);
    if(res == -1)
    {
        return NULL;
    }
    return query(szQuery);
}



bool KaDBConn::execute(const char *sql) {
    if(!m_mysql)
    {
        return false;
    }
    int ret = mysql_real_query(m_mysql,sql,strlen(sql));            //sucess return 0,error rerurn not 0
    if(ret)
    {
        return false;
    }

    return true;
}

uint32_t KaDBConn::getInsertId() {
    return mysql_insert_id(m_mysql);
}

const char * KaDBConn::getPoolName() {
    return m_DBPool->getPoolName();
}

//TODO
int32_t KaDBConn::escapeString(char *szDst, const char *szSrc, uint32_t uSize) {
	// make_the_string more good!

}


/*清除了缓存在客户端中的内存*/
void KaDBConn::clearStoreResults() {
    if(!m_mysql)
    {
        return;
    }
    MYSQL_RES *result = NULL;
    while(!mysql_next_result(m_mysql))
    {
        if((result = mysql_store_result(m_mysql)) != NULL)
        {
            mysql_free_result(result);      //释放存储的内存
        }
    }
}



/*********************************Mysql 连接池************************************************/
KaDBPool::KaDBPool(const char* poolName, const char* db_server_ip, 
                      uint16_t db_server_port, const char* userName,
                      const char* passwd, const char *dbName, int maxConn)
{
    m_PoolName = poolName;
    m_DatabaseInfo.strHost = db_server_ip;
    m_DatabaseInfo.Port = db_server_port;
    m_DatabaseInfo.strUser = userName;
    m_DatabaseInfo.strPwd = passwd;
    m_DatabaseInfo.strDBName = dbName;
    m_db_cur_conn_cnt = MIN_CONN_CNT;
    m_db_max_conn_cnt = maxConn;
}



KaDBPool::~KaDBPool(){
    for(auto iter = m_free_list.begin(); iter != m_free_list.end(); ++iter)
    {
        KaDBConn* temp = *iter;
        delete temp;
    }
    m_free_list.clear();
}


    
int KaDBPool::init(){
    for(int i = 0; i < m_db_cur_conn_cnt; ++i)			//根据当前线程的的最小数量创建指定连接数
    {
        KaDBConn* newConn = new KaDBConn(this);
        int ret = newConn->Init();
        if(ret)
        {
            delete newConn;
            newConn = NULL;
            return -1;
        }
        m_free_list.push_back(newConn);     //创建好的KaDBConn由m_free_list管理
    }
    return 0;           //sucess return 0;
}


/*从线程池中获取一个连接对象*/  
KaDBConn* KaDBPool::getKaConn(){

    std::unique_lock<std::mutex> grand(m_mutex);            //RAII方式使用锁
    while(m_free_list.empty())          //当前链表为空的时候
    {
        if(m_db_cur_conn_cnt >= m_db_max_conn_cnt)          //当前连接大于上限     
        {
            m_cond.wait(grand);         //条件等待        
        }
        else        //没达到连接上限，就创建新的连接
        {
            KaDBConn* pDBConn = new KaDBConn(this);     //当前连接没有满，可以新创建连接。
            int ret = pDBConn->Init();                  //初始化连接
            if(ret)
            {
                delete pDBConn;
                return NULL;
            }
            else
            {
                m_free_list.push_back(pDBConn);         //将创建成功的KaDBConn压入到空闲链表中
                m_db_cur_conn_cnt++;
            }
        }
    }
    KaDBConn* pConn = m_free_list.front();      //管理链表不为空，就在链表中取出一个。
    m_free_list.pop_front();
    return pConn;
}



/*释放某一个连接*/
void KaDBPool::RelKaConn(KaDBConn* pConn){
    std::unique_lock<std::mutex> grand(m_mutex);
    auto iter = m_free_list.begin();
    for(;iter != m_free_list.end(); iter++)
    {
        if(pConn == *iter)
        {
            break;
        }
    }
    if(iter == m_free_list.end())
    {
        m_free_list.push_back(pConn);
    }
    m_cond.notify_all();        //唤醒等待队列上的任意一个，如何没有等待，该函数会怎么样子？
}

//如果调用notify的时候，没有wait的函数会怎么样？


/*******************************线程池管理器***********************************************/
KaDBManager* KaDBManager::g_manager = NULL;


KaDBManager* KaDBManager::getInstance()
{
    if(g_manager != NULL)
    {
        g_manager = new KaDBManager();
        if(g_manager->init())
        {
            delete g_manager;
            g_manager = NULL;
        }
    }
    return g_manager;
}

int KaDBManager::init(){
    // use reader read configure from configural file!
    ConfigFileReader configure("../conf/mysql.conf");
    const char *ip = configure.GetConfigName("KaIM_host");
    const char *port = configure.GetConfigName("");
    uint16_t Port = atoi(port);
    const char *user = configure.GetConfigName("");
    const char *passwd = configure.GetConfigName("");
    const char *dbname = configure.GetConfigName("");
    int max_cnt = atoi(configure.GetConfigName("KaIM_maxconncnt"));
    char DBPoolName[] = "defaultPool";
    //在堆上创建新的连接池
    KaDBPool* pDBPool = new KaDBPool(DBPoolName,ip,Port,user,passwd,dbname,max_cnt);
    if(pDBPool->init())
    {
        return 3;
    }
    m_DBPools.insert(std::make_pair(DBPoolName,pDBPool));
    return 0;
}

/*根据输入的线程池名字，从该线程池中选取出一个线程*/
KaDBConn* KaDBManager::getDBConn(const char* dbPoolName){
    auto it = m_DBPools.find(dbPoolName);
    if(it == m_DBPools.end())
    {
        return NULL;
    }
    else{
        return it->second->getKaConn();
    }
}

KaDBConn* KaDBManager::getDBConn(std::string& dbPoolName){
    return getDBConn(dbPoolName.c_str());
}

//释放连接池，实际上还在调用连接池的API来释放连接池

void KaDBManager::RelDBConn(KaDBConn* pConn){
    if(pConn){
        return;
    }
    auto it = m_DBPools.find(pConn->getPoolName());
    if(it != m_DBPools.end()){
        it->second->RelKaConn(pConn);
    }
}
