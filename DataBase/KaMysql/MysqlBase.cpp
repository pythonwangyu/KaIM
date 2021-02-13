#include "MysqlBase.h"


/*
*			字段类的实现
*/

Field::Field()
:m_Type(DB_TYPE_UNKNOW)
{
	m_NULL = false;
}

Field::Field(const Field& obj)
{
	m_Value = obj.m_Value;
	m_FieldName = obj.m_FieldName;
	m_Type = obj.m_Type;
}


Field::Field(const char* value,DataType type)
:m_Type(type)
{
	m_Value = value;
}


/*
*			QueryResult类的实现
*/

//初始化构造函数
QueryResult::QueryResult(MYSQL_RES* result/*返回句柄*/, uint64_t rowcount/*行数*/, uint32_t fieldCount/*字段数*/)
    :m_FieldCount(fieldCount),m_RowCount(rowcount)
{
	m_Result = result;
	m_CurrentRow = new Field[m_FieldCount];     //在堆中创建一行的数据存储空间
    
    //调用API获取字段的属性
    MYSQL_FIELD *fields = mysql_fetch_fields(m_Result);

    //实际上是对原生的API在一次封装，使得返回的一行数据存储在当前的行上面
    for(uint32_t i  = 0; i < m_FieldCount; ++i )				//初始化字段类
    {
        if(fields[i].name != NULL)
        {
            m_FiledNames[i] = fields[i].name;               	
            m_FiledNameInVector.push_back(fields[i].name);
        }
        else
        {
            m_FiledNames[i] = "";
            m_FiledNameInVector.push_back("");
        }
        m_CurrentRow[i].setType(convertNativeType(fields[i].type));
    }
}


QueryResult::~QueryResult()
{
    endQuery();
}

void QueryResult::endQuery()
{
    if(m_CurrentRow)
    {
        delete [] m_CurrentRow;
        m_CurrentRow = NULL;
    }
    if(m_Result)
    {
        mysql_free_result(m_Result);
        m_Result = NULL;
    }
}


/*
*
*		row 实际上是返回的数据的一个行,这个row是clientMysql自定义的			->
*				
*		row_1 		value_1 		value_2         ......
*		row_2		value_1			value_2			......
*		
*		m_current  是自己封装的返回数据类 ，它将field字段类进一步封装
*			
*		m_current 的核心在于它是一个指向field类数组的指针，这个数组中存放了fieldCount个field类实例，
*		每次调用nextRow函数的时候，会更新m_current,以获取下一行的数据
*/

bool QueryResult::nextRow()
{
    MYSQL_ROW row;
    if(!m_Result)
    {
        return false;
    }
    row = mysql_fetch_row(m_Result);
    if(!row)
    {
        endQuery();
        return false;
    }
    /*获取字段的属性的长度*/
    auto FieldLength = mysql_fetch_lengths(m_Result);

    /*info: m_current已经在初始化的时候在堆上创建了*/
    for(uint32_t i = 0; i < m_FieldCount; ++i)
    {
        if(row[i] == NULL)		//判断是否为空
        {
            m_CurrentRow[i].m_NULL = true;
            m_CurrentRow[i].setValue("",0);
        }
        else
        {
            m_CurrentRow[i].m_NULL = false;
            m_CurrentRow[i].setValue(row[i],FieldLength[i]);
        }
        m_CurrentRow[i].setFiledName(m_FiledNames[i]);
    }
    return true;
}



enum Field::DataType QueryResult::convertNativeType(enum_field_types mysqlType) const
{
    switch (mysqlType)
    {
        case FIELD_TYPE_TIMESTAMP:
        case FIELD_TYPE_DATE:
        case FIELD_TYPE_TIME:
        case FIELD_TYPE_DATETIME:
        case FIELD_TYPE_YEAR:
        case FIELD_TYPE_STRING:
        case FIELD_TYPE_VAR_STRING:
        case FIELD_TYPE_BLOB:
        case FIELD_TYPE_SET:
        case FIELD_TYPE_NULL:
            return Field::DB_TYPE_STRING;
        case FIELD_TYPE_TINY:

        case FIELD_TYPE_SHORT:
        case FIELD_TYPE_LONG:
        case FIELD_TYPE_INT24:
        case FIELD_TYPE_LONGLONG:
        case FIELD_TYPE_ENUM:
            return Field::DB_TYPE_INTEGER;
        case FIELD_TYPE_DECIMAL:
        case FIELD_TYPE_FLOAT:
        case FIELD_TYPE_DOUBLE:
            return Field::DB_TYPE_FLOAT;
        default:
            return Field::DB_TYPE_UNKNOW;
    }
}