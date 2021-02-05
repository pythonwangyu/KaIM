#include "KaDataBaseMysql.h"

#include <cstring>


DataBaseMysql::DataBaseMysql(){
    m_Mysql = NULL;
    m_bInit = false;
}

DataBaseMysql::~DataBaseMysql()
{
    if((m_Mysql != NULL) &&(m_bInit)){
        mysql_close(m_Mysql);
    }
}


//初始化数据库实例
bool DataBaseMysql::initialize(const std::string& host,const std::string& user,const std::string& pwd,const std::string& dbName)
{
    if(m_bInit)
    {
        mysql_close(m_Mysql);
    }
    m_Mysql = mysql_init(m_Mysql);
    m_Mysql = mysql_real_connect(m_Mysql,host.c_str(),
                                user.c_str(),
                                pwd.c_str(),
                                dbName.c_str(),
                                0,
                                NULL,
                                0);
    m_DBInfo.strDBName = dbName;
    m_DBInfo.strHost = host;
    m_DBInfo.strPwd = pwd;
    m_DBInfo.strUser = user;
    if(m_Mysql)
    {
        mysql_query(m_Mysql,"set names uft8");
        m_bInit = true;
        return true;
    }
    else
    {
        mysql_close(m_Mysql);
        return false;
    }
}

/*
*   mysql函数的有两个查询语句：
*       都是成功返回0。
*       失败返回非零。
*   （1） mysql_query(MYSQL* mysql, const char* sql);
*
*   （2） int mysql_real_query(MYSQL *mysql, const char *query, unsigned long length);
*
*   （1）对于mysql_query()函数而言
*   不应为语句添加终结分号（‘;’）或“/g”。
*   如果允许多语句执行，字符串可包含多条由分号隔开的语句。但是连接的时候必须指定CLIENT_MULTI_STATEMENTS选项。
*        不能用于包含二进制数据的查询，
*   应使用mysql_real_query()取而代之（二进制数据可能包含字符‘/0’，mysql_query()会将该字符解释为查询字符串结束）
*   （2）对于mysql_real_query（）函数而言
*
*       
*
*/


//返回查询的结果
QueryResult* DataBaseMysql::query(const char* sql){
    //如果m_Mysql为空，说明还没有初始化
    if(!m_Mysql)
    {
        if(false == initialize(m_DBInfo.strHost,m_DBInfo.strUser,m_DBInfo.strPwd,m_DBInfo.strDBName))
        {
            return NULL;
        }
    }
    if(!m_Mysql)
    {
        return 0;
    }

    MYSQL_RES* result = 0;          //返回的结果指针
    uint64_t rowCount = 0;
    uint32_t fieldCount = 0;
    
    int Res = mysql_real_query(m_Mysql,sql,strlen(sql));

    if(Res)
    {
        unsigned int uError = mysql_errno(m_Mysql);
        if(CR_SERVER_GONE_ERROR == uError)
        {
            if(false == initialize(m_DBInfo.strHost,m_DBInfo.strUser,m_DBInfo.strPwd,m_DBInfo.strDBName))
            {
                return NULL;
            }
            Res = mysql_real_query(m_Mysql,sql,strlen(sql));
            if(Res)
            {
                return NULL;
            }
        }
        else        //其他的错误代码，直接返回就行了
        {
            return NULL;
        }
    }
    result = mysql_store_result(m_Mysql);      //获取查询结果
    rowCount = mysql_affected_rows(m_Mysql);
    fieldCount = mysql_field_count(m_Mysql);

    if(!result)
    {
        return NULL;
    }

    QueryResult* queryResult = new QueryResult(result,rowCount,fieldCount);

    queryResult->nextRow();
    return queryResult;
}



//支持参数

QueryResult* DataBaseMysql::pquery(const char* format,...)
{
    if(!format)
    {
        return NULL;
    }
    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap,format);
    int res = vsnprintf(szQuery,MAX_QUERY_LEN,format,ap);
    va_end(ap);

    if(res == -1)
    {
        return NULL;
    }
    return query(szQuery);

}

bool DataBaseMysql::execute(const char *sql)
{
    if(!m_Mysql)
    {
        return false;
    }
    int Res = mysql_real_query(m_Mysql,sql,strlen(sql));
    if(Res)     //出错了
    {
        unsigned int uError = mysql_errno(m_Mysql);
        if(CR_SERVER_GONE_ERROR == uError)
        {
            //重新连接下
            if(false == initialize(m_DBInfo.strHost,m_DBInfo.strUser,m_DBInfo.strPwd,m_DBInfo.strDBName))
            {
                return false;
            }
            Res = mysql_real_query(m_Mysql,sql,strlen(sql));
            if(Res)
            {
                return false;
            }
            return true;
        }
        else{       //其他的错误
            return false;
        }   
    }
    return true;
}


uint32_t DataBaseMysql::getInsertId(){
    return (uint32_t)mysql_insert_id(m_Mysql);
}

//清除存储的结果
void DataBaseMysql::clearStoredResults(){
    if(!m_Mysql)
    {
        return ;
    }
    MYSQL_RES* result = NULL;
    while(!mysql_next_result(m_Mysql))
    {
        if((result = mysql_store_result(m_Mysql)) != NULL)
        {
            mysql_free_result(result);
        }
    }
}

int32_t DataBaseMysql::escapeString(char* szDst,const char* szSrc, uint32_t uSize){
    if(m_Mysql == NULL)
    {
        return 0;
    }
    if(szDst == NULL || szSrc == NULL)
    {
        return 0;
    }
    return mysql_real_escape_string(m_Mysql,szDst,szSrc,uSize);
}
