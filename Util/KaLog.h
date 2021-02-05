//
// Created by ka on 2021/1/13.
//

#ifndef KANET_KALOG_H
#define KANET_KALOG_H

//c lib
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <cstdarg>

//C++11 lib
#include <string>
#include <list>
#include <thread>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <iostream>

//This the log level
enum LOG_LEVEL
{
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_SYSTEM_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_CRITICAL
};


class  Ka_Log;

#define LOGT(...) Ka_Log::output(LOG_LEVEL_TARCE,__FILE__,__LINE__,__VA_ARGS__)
#define LOGD(...) Ka_Log::output(LOG_LEVEL_DEBUG,__FILE__,__LINE__,__VA_ARGS__)
#define LOGI(...) Ka_Log::output(LOG_LEVEL_INFO,__FILE__,__LINE__,__VA_ARGS__)
#define LOGW(...) Ka_Log::output(LOG_LEVEL_WARNGING,__FILE__,__LINE__,__VA_ARGS__)
#define LOGE(...) Ka_Log::output(LOG_LEVEL_ERROR,__FILE__,__LINE__,__VA_ARGS__)
#define LOGSYSE(...) Ka_Log::output(LOG_LEVEL_SYSTEM_ERROR,__FILE__,__LINE__,__VA_ARGS__)
#define LOGF(...) Ka_Log::output(LOG_LEVEL_FATAL,__FILE__,__LINE__,__VA_ARGS__)
#define LOGC(...) Ka_Log::output(LOG_LEVEL_CRITICAL,__FILE__,__LINE__,__VA_ARGS__)

//This is Easy use for va_list
#define LOGB(...) Ka_Log::EasyDebug(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)


class  Ka_Log{
public:
    Ka_Log()=delete;
    ~Ka_Log()=delete;
    Ka_Log(const Ka_Log& rhs)=delete;
    Ka_Log& operator=(const Ka_Log& rhs)=delete;

    static bool init(const char* LogFileName = nullptr,bool bTrunc = false,int64_t RollSize = 10*1024*1024);
    static void uninit();

    static void setLevel(LOG_LEVEL nLevel);
    static bool isRunning();

    //核心接口
    static bool output(long nLevel,const char* fileName,int nLineNumber,const char* pszFmt,...);
    static void EasyDebug(const char* fileName,const char* functionName,int LineNumber,const char* pszFmt,...);

private:



    static void makeLinePrefix(long nLevel,std::string& strPrxfix);
    static void getTime(char *pszTime,int nTimeStrLength);
    static bool createNewFile(const char* pszLogFileName);
    static bool writeToFile(const std::string& data);

    static void crash();

    static void writeThreadProc();




private:
    static bool                                 m_bToFile;
    static  FILE*                               m_hLogFile;
    static std::string                          m_FileName;
    static std::string                          m_FileNamePid;
    static bool                                 m_bTrunc;
    static LOG_LEVEL                            m_nCurrentLevel;
    static int64_t                              m_MaxLogSize;
    static int64_t                              m_CurrentWrittenSize;
    static std::list<std::string>               m_listLine;         //待写入的日志内容
    static std::unique_ptr<std::thread>         m_spWriteThread;
    static std::mutex                           m_mutexWrite;


    static std::condition_variable              m_CV;
    static bool                                 m_Exit;
    static bool                                 m_Running;
};


#endif //KANET_KALOG_H
