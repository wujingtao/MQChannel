#ifndef ALARMSERVER_H
#define ALARMSERVER_H

#include <qthread.h>
#include "param.h"

//刷新不正确报警数据
class AlarmServer : public QThread
{
public:
    AlarmServer();

public:
    void run();

private:
    int getRefreshSecond()
    {
        return REFRESH_DISCORRECT_ALARM_SECOND;
    }

private:
    int m_maxID;
    int m_alarmDiscorrectCount;
};

#endif // ALARMSERVER_H
