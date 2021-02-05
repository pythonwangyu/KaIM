#ifndef __KAQUERYRESULT_H__
#define __KAQUERYRESULT_H__


//查询结果类

//			file_name_1   file_name_2    file_name_3   .......
//		row1	value_1	      value_2        value_3	   .......
//		row2	.......
	

#include <mysql/mysql.h>
#include <map>
#include <vector>


#include "KaField.h"


class QueryResult{

	typedef std::map<uint32_t,std::string> FieldNames;          //Field_index map

public:
	QueryResult(MYSQL_RES* result,uint64_t rowCount, uint32_t fieldCount);
	virtual ~QueryResult();
	virtual bool nextRow();			

    uint32_t getField_idx(const std::string& name) const   //获取属性的序号
    {
        for(auto iter = m_FieldNames.begin();iter != m_FieldNames.end(); ++iter)
        {
            if(iter->second == name)
            {
                return iter->first;
            }
        }
        return uint32_t(-1);
    }

    //获取当前行
    Field* fetch()const {
        return m_CurrentRow;
    }

    //根据index获取当前行，根据FiledName获取当前行
    const Field& operator[] (int index) const{
        return m_CurrentRow[index];
    }

    const Field& operator[] (std::string name) const{
        return m_CurrentRow[getField_idx(name)];
    }

    void endQuery();


public:
    //获取查询结果的列数
    uint32_t getFieldCount() const {
        return m_FieldCount;
    }
    //获取查询结果的行数
    uint64_t getRowCount() const{
        return m_RowCount;
    }

    FieldNames const&  getFieldNames() const {
        return m_FieldNames;
    }
    //获取所有查询结构的行数
    std::vector<std::string> const& getNames() const{
        return m_FieldNames_V;
    }
    
private:
    enum Field::DataType convertNativeType(enum_field_types mysqlType)const;

protected:

    Field*                          m_CurrentRow;       //指向当前行的指针
    uint32_t                        m_FieldCount;       //属性的数量
    u_int64_t                       m_RowCount;         //数据行的数量
    FieldNames                      m_FieldNames;       //获取数据的
    std::vector<std::string>        m_FieldNames_V;     //
    
    MYSQL_RES*                      m_Result;;

};




#endif
