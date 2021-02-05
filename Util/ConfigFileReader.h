//
// Created by ka on 2021/1/13.
//

#ifndef KANET_CONFIGFILEREADER_H
#define KANET_CONFIGFILEREADER_H

/*
 * This class is for read config file,it's useful
 *  in fact,it uses std::map to store the key value as config
 * */

#include <map>
#include <string>

class ConfigFileReader {
public:
    ConfigFileReader();
    ~ConfigFileReader();

    char* GetConfigName(const char* name);
    int SetConfigValue(const char* name,const char* value);

private:
    /*private function*/

    void LoadFile(const char* fileName);
    int WriteToFile(const char* fileName = nullptr);
    void ParseLine(char* line);
    char* TrimSpace(char* name);


private:
    bool                                m_has_load;
    std::map<std::string,std::string>   m_config_map;
    std::string                         m_configfile;
};


#endif //KANET_CONFIGFILEREADER_H
