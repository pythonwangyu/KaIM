#include "KaField.h"

Field::Field()
    :m_Type(DB_TYPE_UNKNOWN)
{
    m_Null = false;
}

Field::Field(const Field &f){
    m_Value = f.m_Value;
    m_FieldName = f.m_FieldName;
    m_Type = f.getType();
}
Field::Field(const char* value,enum DataType type)
    :m_Type(type)
{
    m_Value = value;
}

Field::~Field()
{

}
