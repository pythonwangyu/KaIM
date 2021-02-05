#ifndef __KADATABASEMYSQL_H__
#define __KADATABASEMYSQL_H__

#include <cstdint>
#include <mysql/mysql.h>
#include <mysql/errmsg.h>
#include "KaQueryResult.h"

#define MAX_QUERY_LEN 1024

class DataBaseMysql{
public:
    struct DatabaseInfo{            //数据库的信息
        std::string strHost;
        std::string strUser;
        std::string strPwd;
        std::string strDBName;
    };

public:
    DataBaseMysql();
    ~DataBaseMysql();
    
    bool initialize(const std::string& host,const std::string& user,const std::string& pwd,const std::string& dbName);
    
    //查询专用的函数
    QueryResult* query(const char* sql);
    QueryResult* query(const std::string& sql){
        return query(sql.c_str());
    }

    //格式化的查询语句输入
    QueryResult* pquery(const char* format,...);

    //执行专用的函数
    bool execute(const char* sql);
    //下面两个未实现
    bool execute(const char* sql,uint32_t& Count,int& nErrno);
    bool pexecute(const char* format,...);
    
    uint32_t getInsertId();

    void clearStoredResults();

    int32_t escapeString(char* szDst,const char* szSrc, uint32_t uSize);

private:
    DatabaseInfo        m_DBInfo;           //数据库信息
    MYSQL*              m_Mysql;            //mysql连接返回的MYSQL结构体
    bool                m_bInit;            //是否初始化
};

#endif
