#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <string>
using std::string;

enum LogLevel
{
    LL_ALL,
    LL_DEBUG,
    LL_INFO,
    LL_WARN,
    LL_ERROR,
    LL_OFF,
};

#define MAX_LOG_LENTH 1024

class Logger
{
public:
    Logger();
    ~Logger();

public:
    static void init(const char *prefix, LogLevel leve);
    static int open(const char *path, const char *mode);
    static int close();

    static void print(LogLevel level, char *format, ...);

    static LogLevel getLogLevel();
    static void setLogLevel(LogLevel level);

private:
    static FILE *m_fp;
    static LogLevel m_logLevel;
    static char* m_logLevelName[LL_OFF];

    static int m_timestamp;
    static string m_prefix;
};

#define LOG_DEBUG(...) Logger::print(LL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)  Logger::print(LL_INFO,  __VA_ARGS__)
#define LOG_WARN(...)  Logger::print(LL_WARN,  __VA_ARGS__)
#define LOG_ERROR(...) Logger::print(LL_ERROR, __VA_ARGS__)

#endif // LOGGER_H
