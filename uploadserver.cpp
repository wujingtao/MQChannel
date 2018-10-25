#include "uploadserver.h"
#include "qmyglobal.h"
#include "datatranslate.h"
#include "AlarmManager.h"

UploadServer::UploadServer()
{

}

void UploadServer::run()
{
    while(true)
    {
        mAlarmRecord_stu record;
        if(!ALARM_MANAGER->PopAlarmRecord(record))
        {
            msleep(1000);
            continue;
        }

        if(!upload(record))
        {
            ALARM_MANAGER->PushAlarmRecord(record);
        }
    }
}

bool UploadServer::upload(mAlarmRecord_stu &record)
{
    QString statusCode;
    //upload jpg
    if(record.flag==1)
    {        
        if(!NetWorkSendManager::Instance()->saveAlarmRecord(record, statusCode))
        {
            return false;
        }

        record.flag=2;
    }

    //upload mp4
    if(record.flag==2)
    {
        QString filePath = record.filePath;
        QString mp4FileName = filePath.replace(".jpg",".mp4");
        if(!ALARM_MANAGER->CreateMp4(record.cameraID, mp4FileName))
        {
            return false;
        }

        if(!NetWorkSendManager::Instance()->saveAlarmMP4(mp4FileName,statusCode,record.alarm_id,record.rule_id))
        {
            return false;
        }
    }

    return true;
}
//
