#include "Logger.h"

#include "Logger.h"

#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

//test
#include <qdatetime.h>


FILE* Logger::m_fp = stdout;
LogLevel Logger::m_logLevel = LL_ALL;
char* Logger::m_logLevelName[LL_OFF] = {"", "DEBUG", "INFO", "WARN", "ERROR"};
int Logger::m_timestamp=0;
string Logger::m_prefix="";

Logger::Logger()
{
}

Logger::~Logger()
{

}

void Logger::init(const char *prefix, LogLevel leve)
{
    char szFile[260] = {};
    mkdir("log", 0777);

    time_t nowtime = time(NULL);
    m_timestamp = (int)nowtime;
    tm time = *localtime(&nowtime);

    snprintf(szFile, sizeof(szFile)-1, "log/%s.%d.%d.%d.%d.%d.%d.%d.log", prefix, time.tm_year+1900, time.tm_mon+1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, getpid());
    open(szFile, "a");
    setLogLevel(leve);
    m_prefix=prefix;
}

int Logger::open(const char *path, const char *mode)
{
    if (m_fp != NULL && m_fp != stdout && m_fp != stderr)
    {
        fclose(m_fp);
        m_fp = NULL;
    }

    FILE *fp = fopen(path, mode);
    if (fp == NULL)
    {
        return errno;
    }

    m_fp = fp;
    return 0;
}

int Logger::close()
{
    if (m_fp != NULL && m_fp != stdout && m_fp != stderr)
    {
        int err = fclose(m_fp);
        return err;
    }

    return 0;
}

void Logger::print(LogLevel level, char *format, ...)
{
    if (m_fp != NULL && level >= m_logLevel && level < LL_OFF)
    {
        QString nowtime = QDateTime::currentDateTime().toString("hh:mm:ss:zzz");

        int timestamp = static_cast<int>(time(NULL));
        char szLog[MAX_LOG_LENTH] = {};
        //int offset = snprintf(szLog, sizeof(szLog)-1, "[%10d][%5s] ", timestamp, m_logLevelName[level]);
        int offset = snprintf(szLog, sizeof(szLog)-1, "[%12s][%5s] ", nowtime.toStdString().c_str(), m_logLevelName[level]);

        if (offset > 0 && offset < static_cast<int>(sizeof(szLog)-1))
        {
            va_list vl;
            va_start(vl, format);
            offset += vsnprintf(szLog+offset, sizeof(szLog)-1-offset, format, vl);
            if (offset > 0)
            {
                fputs(szLog, m_fp);
                fflush(m_fp);
            }
        }

        //TODO:debug test
        if(level==LL_DEBUG && timestamp>(m_timestamp+86400))
        {
            init(m_prefix.c_str(), m_logLevel);
        }
    }
}

LogLevel Logger::getLogLevel()
{
    return m_logLevel;
}

void Logger::setLogLevel(LogLevel level)
{
    if (level >= LL_ALL && level < LL_OFF)
    {
        m_logLevel = level;
    }
}

