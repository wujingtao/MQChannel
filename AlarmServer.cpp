#include "AlarmServer.h"
#include "NetWorkManager.h"
#include "AlarmManager.h"

AlarmServer::AlarmServer()
{
    m_maxID=0;
    m_alarmDiscorrectCount=0;
}

void AlarmServer::run()
{    
    while(OPEN_DISCORRECT_ALARM)
    {
        int startTime = time(NULL);

        int maxID=0, alarmDiscorrectCount=0;
        NETWORK_MANAGER->HasAlarmDiscorrect(maxID, alarmDiscorrectCount);
        if(maxID != m_maxID || alarmDiscorrectCount != m_alarmDiscorrectCount)
        {
            ALARM_MANAGER->ClearAlarmDiscorrect();
            NETWORK_MANAGER->GetAlarmDiscorrect();
            m_maxID=maxID;
            m_alarmDiscorrectCount=alarmDiscorrectCount;
        }

        int durtime = startTime+getRefreshSecond()-time(NULL);
        if(durtime>0)
        {
            msleep(durtime*1000);
        }

    }
}
