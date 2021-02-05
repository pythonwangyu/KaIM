#include "KaQueryResult.h"

QueryResult::QueryResult(MYSQL_RES* result,uint64_t rowCount, uint32_t fieldCount)
    :m_FieldCount(fieldCount),m_RowCount(rowCount)
{
    //m_FieldCount表示列的数量
    m_Result = result;  //返回结构体
    m_CurrentRow = new Field[m_FieldCount];
    
    //返回field数组
    MYSQL_FIELD *fields = mysql_fetch_fields(m_Result);
    //m_FieldCount，表示列的数目，
    for(uint32_t i = 0; i < m_FieldCount; ++i)
    {
        if(fields[i].name != NULL)
        {
            m_FieldNames[i] = fields[i].name;
            m_FieldNames_V.push_back(fields[i].name);
        }
        else
        {
            m_FieldNames[i] = "";
            m_FieldNames_V.push_back("");
        }
        m_CurrentRow[i].setType(convertNativeType(fields[i].type));
    }
}


QueryResult::~QueryResult()
{
    endQuery();
}


bool QueryResult::nextRow()
{
    //row就是当前的行数据
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
    unsigned long int *ulFieldLength;
    ulFieldLength = mysql_fetch_lengths(m_Result);
    for(uint32_t i = 0; i < m_FieldCount; ++i)
    {
        if(row[i] == NULL)
        {
           m_CurrentRow[i].m_Null = true;
           m_CurrentRow[i].setValue("",0);
        }
        else
        {
            m_CurrentRow[i].m_Null = false;
            m_CurrentRow[i].setValue(row[i],ulFieldLength[i]);
        }
        m_CurrentRow[i].setName(m_FieldNames[i]);
    }
    return true;
}


void QueryResult::endQuery()
{
    if(m_CurrentRow)
    {
        delete [] m_CurrentRow;
        m_CurrentRow = 0;
    }
    if(m_Result)
    {
        mysql_free_result(m_Result);
        m_Result = 0;
    }
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
            return Field::DB_TYPE_UNKNOWN;
    }
}
