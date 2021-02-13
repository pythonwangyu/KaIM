#ifndef _MYSQLBASE_H__
#define _MYSQLBASE_H__


#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <mysql/mysql.h>

/*
*	字段类， field类别
*   使用field类别来识别每一个数据，每一个数据有他的1、类型；2、名字；3、值
*
*           | fieldName_1 | fieldName_2 | fieldName_3 |  ......  | fieldName_n |
*   row_1   |  Value_1    |   value_2   |  value_3    |  ......  |   value_n   |
*   row_2   |  Value_1    |   value_2   |  value_3    |  ......  |   value_n   |
*   row_3   |  Value_1    |   value_2   |  value_3    |  ......  |   value_n   |
*   row_4   |  Value_1    |   value_2   |  value_3    |  ......  |   value_n   |
*   row_5   |  Value_1    |   value_2   |  value_3    |  ......  |   value_n   |
*   .............................................................................
*   row_n   |  Value_1    |   value_2   |  value_3    |  ......  |   value_n   |
*
*       其中一个field类其实是对应了一个三元组 [fieldName, fieldType, fieldValue]
*       因此，一个field数组就可以表示一行返回的结果
*/


class Field{
public:
	/*字段的类型，这里只是简单的分成了五类*/
	enum DataType{
		DB_TYPE_UNKNOW 	= 0x00,
		DB_TYPE_STRING 	= 0x01,
		DB_TYPE_INTEGER = 0x02,
		DB_TYPE_FLOAT 	= 0x03,
		DB_TYPE_BOOL 	= 0x04
	};

	Field();
	Field(const Field& obj);
	Field(const char* value,enum DataType type);
	~Field() = default;

	DataType getType() const{ return m_Type;}

	std::string getStringValue() {return m_Value;}

	float getFloatValue() {
		return static_cast<float>(atof(m_Value.c_str()));
	}

	bool getBoolValue(){
		return atoi(m_Value.c_str());
	}

	int32_t getInt32Value() const {
        return static_cast<int32_t>(atol(m_Value.c_str()));
    }
    uint32_t getUInt32Value() const {
        return static_cast<uint32_t>(atol(m_Value.c_str()));
    }

    uint8_t getUInt8Value() const {
        return static_cast<uint8_t>(atol(m_Value.c_str()));
    }

    uint16_t getUInt16Value() const {
        return static_cast<uint16_t>(atol(m_Value.c_str()));
    }
    int16_t getInt16Value() const {
        return static_cast<int16_t>(atol(m_Value.c_str()));
    }
    
    uint64_t getUInt64Value() const{
        uint64_t value = 0;
        value = atoll(m_Value.c_str());
        return value;
    }

    void setType(enum DataType type){
        m_Type = type;
    }


    void setValue(const char* value,size_t len)
    {
        m_Value.assign(value,len);
    }

    void setFiledName(const std::string& strName)
    {
        m_FieldName = strName;
        toLowerString(m_FieldName);
    }
    const std::string& getFieldName(){
        return m_FieldName;
    }

    bool isNull()			//是否为空？
    {
        return m_NULL;
    }

	bool 				m_NULL;
private:
	std::string   		m_FieldName;		//字段的名字
	std::string 		m_Value;			//字段的值
	DataType			m_Type;				//字段的值的类型
};




/*******************************************************************************************************/
/*
*	QueryResult 类 
*		用于存储查询的结果，并且会将返回的字段名存储在map和vector中。
*		
*/




typedef std::map<uint32_t,std::string> FieldNames;      //index -> FieldName

class QueryResult{
public:
    QueryResult(MYSQL_RES* result,uint64_t rowCount, uint32_t fieldCount);

    ~QueryResult();

    bool nextRow();

    uint32_t getField_idx(const std::string& name) const {
        for(auto it = m_FiledNames.begin(); it != m_FiledNames.end(); ++it)
        {
            if(it->second == name)
            {
                return it->first;
            }
        }
        return uint32_t(0);
    }

    Field* fetch() const {
        return m_CurrentRow;
    }

    const Field& operator[](int index)const{
        return m_CurrentRow[index];
    }

    const Field& operator[](std::string name) const{
        return  m_CurrentRow[getField_idx(name)];
    }

    void endQuery();


public:
    uint32_t getFieldCount() const{
        return m_FieldCount;
    }

    uint64_t getRowCount() const{
        return m_RowCount;
    }

    FieldNames getFieldNames(){
        return m_FiledNames;
    }

    std::vector<std::string> getNames() const{
        return m_FiledNameInVector;         //获取所有的字段，存储在vector中
    }

    Field::DataType convertNativeType(enum_field_types mysqlType) const;
private:
    Field*                      m_CurrentRow;           //当前行的field数组，这个在初始化的时候在堆上创建
    uint32_t                    m_FieldCount;           //当前结果的field的数量
    u_int64_t                   m_RowCount;             //返回结果行数
    FieldNames                  m_FiledNames;

    /* typedef std::map<uint32_t,std::string> FieldNames;      //index -> FieldName */

    std::vector<std::string>    m_FiledNameInVector;	//返回的字段的名字
    MYSQL_RES*                  m_Result;				//mysql返回句柄
};



#endif