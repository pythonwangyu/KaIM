//
// Created by ka on 2021/1/13.
//

#include "ConfigFileReader.h"
#include <cstdio>
#include <cstring>


ConfigFileReader::ConfigFileReader(const char* fileName)
{
    LoadFile(fileName);
}


ConfigFileReader::~ConfigFileReader()
{}

char* ConfigFileReader::GetConfigName(const char* name)
{
    if(!m_has_load)
    {
        return NULL;
    }
    char* value = NULL;
    auto it = m_config_map.find(name);
    if(it != m_config_map.end())
    {
        value = (char*)it->second.c_str();
    }
    return value;
}



int ConfigFileReader::SetConfigValue(const char* name,const char *value)
{
    if(!m_has_load)
    {
        return -1;
    }
    auto it = m_config_map.find(name);
    if(it != m_config_map.end())
    {
        it->second = value;
    }
    else
    {
        m_config_map.insert(std::make_pair(name,value));
    }
    return WriteToFile();
}

void ConfigFileReader::LoadFile(const char* fileName)
{
    m_configfile.clear();
    m_configfile.append(fileName);
    FILE* fp = fopen(fileName,"r");
    if(!fp)
    {
        return;
    }
    char buf[256];
    
    //逐行从配置文件中读取数据
    while(1)
    {
        char* p = fgets(buf,256,fp);
        if(!p)
        {
            break;
        }
        size_t len = strlen(buf);
        if(buf[len-1] == '\n')
        {
            buf[len-1] = 0;
        }
        char *ch = strchr(buf,'#');
        if(ch){
            *ch = 0;
        }
        if(strlen(buf) == 0){
            continue;
        }
        ParseLine(buf);
    }
    fclose(fp);
    m_has_load = true;
}


int ConfigFileReader::WriteToFile(const char* fileName)
{
    FILE* fp = NULL;
    if(fileName == NULL)
    {
        fp = fopen(m_configfile.c_str(),"w");
    }
    else
    {
        fp = fopen(fileName,"w");
    }
    if(fp == NULL)
    {
        return -1;
    }
    char szPaire[128];
    auto it = m_config_map.begin();
    for(; it != m_config_map.end();++it)
    {
        memset(szPaire,0,sizeof(szPaire));
        snprintf(szPaire,sizeof(szPaire),"%s=%s\n",it->first.c_str(),it->second.c_str());
        size_t ret = fwrite(szPaire,strlen(szPaire),1,fp);
        if(ret != 1)
        {
            fclose(fp);
            return -1;
        }
    }
    fclose(fp);
    return 0;
}


void ConfigFileReader::ParseLine(char *line)
{
    char *p  = strchr(line,'=');
    if(p == NULL)
    {
        return;
    }
    *p = 0;
    char* key = TrimSpace(line);
    char* value = TrimSpace(p+1);
    if(key && value)
    {
        m_config_map.insert(std::make_pair(key,value));
    }
}

char* ConfigFileReader::TrimSpace(char* name)
{
    char* start_pos = name;
    while((*start_pos == ' ')||(*start_pos == '\t')||(*start_pos == '\r'))
    {
        start_pos++;
    }
    char* end_pos = name + strlen(name) - 1;
    while((*end_pos == ' ')||(*end_pos == '\t')||(*end_pos == '\r'))
    {
        *end_pos = 0;
        end_pos--;
    }
    int len = (int)(end_pos - start_pos) + 1;
    if(len <= 0)
    {
        return NULL;
    }
    return start_pos;
}
    
