//
// Created by ka on 2021/1/13.
//

#include "KaLog.h"
#include <cstdarg>
#include <sys/timeb.h>



#include <iostream>
#include <sstream>


#define DEFAULT_ROLL_SIZE 10*1024*1024
#define MAX_LINE_LENGTH 256




//静态初始化
bool Ka_Log::m_bTrunc = false;
FILE* Ka_Log::m_hLogFile = nullptr;

std::string Ka_Log::m_FileName;
std::string Ka_Log::m_FileNamePid;

LOG_LEVEL Ka_Log::m_nCurrentLevel = LOG_LEVEL_INFO;
int64_t Ka_Log::m_MaxLogSize = DEFAULT_ROLL_SIZE;
int64_t Ka_Log::m_CurrentWrittenSize = 0;

std::list<std::string> Ka_Log::m_listLine;
std::unique_ptr<std::thread> Ka_Log::m_spWriteThread;
std::mutex Ka_Log::m_mutexWrite;
std::condition_variable Ka_Log::m_CV;
bool Ka_Log::m_Exit = false;
bool Ka_Log::m_Running = false;

bool Ka_Log::m_bToFile = true;

void Ka_Log::EasyDebug(const char *fileName, const char *functionName, int LineNumber, const char *pszFmt, ...) {
    std::string strLine;
    char szFileName[512] = {0};

    snprintf(szFileName,sizeof(szFileName),"[%s文件,第%d行,%s函数]: ",fileName,LineNumber,functionName);
    strLine += szFileName;

    //#include <starg.h>
    va_list  ap;
    va_start(ap,pszFmt);
    int nLoglength = vsnprintf(NULL,0,pszFmt,ap);
    va_end(ap);

    std::string strLogMsg;
    if(static_cast<int>(strLogMsg.capacity()) < nLoglength+1)
    {
        strLogMsg.resize(nLoglength + 1);
    }

    va_list aq;
    va_start(aq,pszFmt);
    snprintf((char*)strLogMsg.data(),strLogMsg.capacity(),pszFmt,aq);
    va_end(aq);

    std::string strMsgFormal;
    strMsgFormal.append(strLogMsg.c_str(),nLoglength);

    strLine += strMsgFormal;
    std::cout << strLine << std::endl;
}


bool Ka_Log::init(const char *LogFileName, bool bTrunc, int64_t RollSize) {
    m_bTrunc = bTrunc;
    m_MaxLogSize = RollSize;

    if(LogFileName == nullptr || 0 == LogFileName[0])
    {
        m_FileName.clear();             //文件名
    }
    else
    {
        m_FileName = LogFileName;
    }

    char PID[8] = {0};
    snprintf(PID,sizeof(PID),"%05d",(int)::getpid());
    m_FileNamePid = PID;

    m_spWriteThread.reset(new std::thread(writeThreadProc));
    return true;
}


void Ka_Log::uninit() {
    m_Exit = true;
    m_CV.notify_one();
    if(m_spWriteThread->joinable())
    {
        m_spWriteThread->join();
    }
    if(m_hLogFile != nullptr)
    {
        fclose(m_hLogFile);
        m_hLogFile = nullptr;
    }
}


void Ka_Log::setLevel(LOG_LEVEL nLevel) {
    if(nLevel < LOG_LEVEL_TRACE || nLevel > LOG_LEVEL_FATAL)
    {
        return;
    }
    m_nCurrentLevel = nLevel;
}


bool Ka_Log::isRunning() {
    return m_Running;
}



bool Ka_Log::output(long nLevel, const char *fileName, int nLineNumber, const char *pszFmt, ...) {
    if(nLevel != m_nCurrentLevel)
    {
        if(nLevel < m_nCurrentLevel)
        {
            return false;
        }
    }
    std::string strLine;

    //预处理
    makeLinePrefix(nLevel,strLine);
    char szFileName[512] = {0};
    snprintf(szFileName,sizeof(szFileName),"[%s文件:第%d行]",fileName,nLineNumber);
    strLine += szFileName;


    //Log 正文
    std::string strLogMsg;

    va_list ap;
    va_start(ap,pszFmt);
    int nLogMsgLength = vsnprintf(nullptr,0,pszFmt,ap);
    va_end(ap);

    if(static_cast<int>(strLogMsg.capacity()) < nLogMsgLength+1)
    {
        strLogMsg.resize(nLogMsgLength+1);
    }

    va_list aq;
    va_start(aq,pszFmt);

    vsnprintf((char*)strLogMsg.data(),strLogMsg.capacity(),pszFmt,aq);

    va_end(aq);

    std::string strMsgFormal;

    strMsgFormal.append(strLogMsg.c_str(),nLogMsgLength);


    if(m_bTrunc)
    {
        strMsgFormal = strMsgFormal.substr(0,MAX_LINE_LENGTH);
    }

    strLine += strMsgFormal;

    if(!m_FileName.empty())
    {
        strLine += "\n";
    }
    if(nLevel != LOG_LEVEL_FATAL)
    {
        std::lock_guard<std::mutex> lock_guard(m_mutexWrite);
        m_listLine.push_back(strLine);
        m_CV.notify_one();
    }
    else
    {
        std::cout << strLine << std::endl;
        if(!m_FileName.empty())
        {
            if(nullptr == m_hLogFile)
            {
                //创建一个新的日志文件夹
                char szNow[64];
                time_t now = time(NULL);
                tm time{};
                localtime_r(&now,&time);
                strftime(szNow,sizeof(szNow),"%Y%m%d%H%M%S",&time);

                std::string strNewFileName(m_FileName);
                strNewFileName += ".";
                strNewFileName += szNow;
                strNewFileName += ".";
                strNewFileName += m_FileNamePid;
                strNewFileName += ".log";
                if(!createNewFile(strNewFileName.c_str()))
                {
                    return false;
                }
            }
            writeToFile(strLine);
        }
        //写入之后让程序挂掉
        crash();
    }
    return true;
}




//pre_str           【info】[Time][ThreadInfo]
void Ka_Log::makeLinePrefix(long nLevel, std::string &strPrxfix) {
    strPrxfix = "[INFO]";
    if(nLevel == LOG_LEVEL_TRACE)
    {
        strPrxfix = "[TARCE]";
    }
    else if(nLevel == LOG_LEVEL_DEBUG)
    {
        strPrxfix = "[DEBUG]";
    }
    else if(nLevel == LOG_LEVEL_WARNING)
    {
        strPrxfix = "[WARNING]";
    }
    else if(nLevel == LOG_LEVEL_ERROR)
    {
        strPrxfix = "[ERROR]";
    }
    else if(nLevel == LOG_LEVEL_SYSTEM_ERROR)
    {
        strPrxfix = "[SYS_ERROR]";
    }
    else if(nLevel == LOG_LEVEL_FATAL)
    {
        strPrxfix = "[SYS_FATAL]";
    }
    else if(nLevel == LOG_LEVEL_CRITICAL)
    {
        strPrxfix = "[CRIRICAL]";
    }

    char szTime[64]  = {0};

    getTime(szTime,sizeof(szTime));
    strPrxfix += "[";
    strPrxfix += szTime;
    strPrxfix += "]";
    char szThreadId[32] = {0};
    std::ostringstream osThreadID;
    osThreadID << std::this_thread::get_id();
    snprintf(szThreadId,sizeof(szThreadId),"[%s]",osThreadID.str().c_str());
    strPrxfix += szThreadId;
}




void Ka_Log::getTime(char *pszTime, int nTimeStrLength) {
    struct timeb tp;
    ftime(&tp);

    time_t now = tp.time;
    tm time{};

    localtime_r(&now,&time);

    snprintf(pszTime,nTimeStrLength,"[%04d-%02d-%02d %02d:%02d:%02d:%03d]",
             time.tm_year+1900,time.tm_mon+1,time.tm_mday,time.tm_hour,time.tm_min,time.tm_sec,
             tp.millitm);
}

bool Ka_Log::createNewFile(const char *pszLogFileName) {
    if(m_hLogFile != nullptr)
    {
        fclose(m_hLogFile);
    }
    m_hLogFile = fopen(pszLogFileName,"aw+");
    return m_hLogFile != nullptr;
}


bool Ka_Log::writeToFile(const std::string &data) {
    std::string strLocal(data);
    int ret = 0;
    while(true)
    {
        ret = fwrite(strLocal.c_str(),1,strLocal.length(),m_hLogFile);
        if(ret <= 0)
        {
            return false;
        }
        else if(ret <= static_cast<int>(strLocal.length()))
        {
            strLocal.erase(0,ret);
        }

        if(strLocal.empty())
            break;
    }
    fflush(m_hLogFile);
    return true;
}



void Ka_Log::crash() {
    char *p = nullptr;
    *p = 0;
}



/*核心，写入线程函数*/
void Ka_Log::writeThreadProc() {
    m_Running = true;
    while(true)
    {
        if(!m_FileName.empty())     //如果文件名不为空
        {
            if(m_hLogFile == nullptr || m_CurrentWrittenSize >= m_MaxLogSize)
            {
                //如果文件为空，或者写入的日志超过日志文件的最大范围
                char szNow[64] = {0};
                time_t now = time(NULL);
                tm time;
                localtime_r(&now,&time);

                strftime(szNow,sizeof(szNow),"%Y-%m-%d[%H:%M:%S]",&time);

                std::string strNewFileName(m_FileName);
                strNewFileName += ".";
                strNewFileName += szNow;
                strNewFileName += ".";
                strNewFileName += m_FileNamePid;
                strNewFileName += ".log";
                if(!createNewFile(strNewFileName.c_str()))  return;
            }
        }

        std::string strLine;
        {
            std::unique_lock<std::mutex> guard(m_mutexWrite);
            while(m_listLine.empty())
            {
                if(m_Exit)
                    return;
                m_CV.wait(guard);
            }
            strLine = m_listLine.front();
            m_listLine.pop_front();
        }

        std::cout << strLine << std::endl;

        if(!m_FileName.empty())
        {
            if(!writeToFile(strLine))
                return;
            m_CurrentWrittenSize += strLine.length();
        }
    }
    m_Running = false;
}