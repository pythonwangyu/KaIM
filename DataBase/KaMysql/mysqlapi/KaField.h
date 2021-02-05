#ifndef __KAFIELD_H__
#define __KAFIELD_H__

//每一列的属性
#include <algorithm>
#include <string>

//change to lower letter
inline void toLowerString(std::string& str)
{
    for(size_t i = 0; i < str.size() ;++i)
    {
        if(str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = str[i] + ('a' - 'A');
        }
    }
}


class Field{
public:
    enum DataType
    {
        DB_TYPE_UNKNOWN = 0x00,
        DB_TYPE_STRING  = 0x01,
        DB_TYPE_INTEGER = 0x02,
        DB_TYPE_FLOAT   = 0x03,
        DB_TYPE_BOOL    = 0x04
    };
    Field();
    Field(const Field &f);
    Field(const char* value,enum DataType type);
    ~Field();

    enum DataType getType() const {return m_Type;}
    const std::string getString() const { return m_Value; }
    std::string getNoConstString() const { return m_Value;  }
    
    //atof():change a char* to float;
    float getFloat() const {
        return static_cast<float>(atof(m_Value.c_str()));
    }

    //atoi :change char* to int
    bool getBool() const {
        return atoi(m_Value.c_str()) > 0;
    }

    int32_t getInt32() const {
        return static_cast<int32_t>(atol(m_Value.c_str()));
    }
    uint32_t getUInt32() const {
        return static_cast<uint32_t>(atol(m_Value.c_str()));
    }

    uint8_t getUInt8() const {
        return static_cast<uint8_t>(atol(m_Value.c_str()));
    }

    uint16_t getUInt16() const {
        return static_cast<uint16_t>(atol(m_Value.c_str()));
    }
    int16_t getInt16() const {
        return static_cast<int16_t>(atol(m_Value.c_str()));
    }
    
    uint64_t getUInt64() const{
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

    void setName(const std::string& strName)
    {
        m_FieldName = strName;
        toLowerString(m_FieldName);
    }

    const std::string& getName(){
        return m_FieldName;
    }

    bool isNull()
    {
        return m_Null;
    }
    bool                m_Null;
private:
    std::string         m_FieldName;                //列的名字
    std::string         m_Value;                    //列的值
    enum  DataType      m_Type;                     //列的属性
};

#endif
