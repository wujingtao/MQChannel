#include "datatranslate.h"
#include <QDebug>
#include <QBuffer>
#include <QDateTime>

dataTranslate* dataTranslate::s_translate=NULL;

dataTranslate::dataTranslate()
{

}

bool dataTranslate::login(QString &retcode)
{
    bool ret= NetWorkSendManager::Instance()->getLogin(userName,passWD,retcode);
    return ret;
}

//获取摄像头配置
void dataTranslate::getCameraConfig(QString server_id,QString &retcode)
{
    NetWorkSendManager::Instance()->getCameraList(server_id,retcode);
}

//获取摄像头规则
void dataTranslate::getCameraRule(QString cameraId,QString &retcode)
{
    //1获取规则列表
    map_CutImageRule.clear();
    cameraId="";
    NetWorkSendManager::Instance()->getAlarmRule(cameraId,retcode);
}

//获取摄像头RTSP
void dataTranslate::getCameraRTSP(QStringList cameraIdlist,QString &retcode)
{
    //NetWorkSendManager::Instance()->getCameraRTSPList(cameraIdlist,retcode);
}

//获取画线类型
void dataTranslate::getLineType(QString &retcode)
{
    QString dicType="line_type";
    NetWorkSendManager::Instance()->getDictionaryList(dicType,retcode);
}

//获取告警类型
void dataTranslate::getAlarmType(QString &retcode)
{
    QString dicType="alarm_type";
    NetWorkSendManager::Instance()->getDictionaryList(dicType,retcode);
}

//插入新规则或更新旧规则
void dataTranslate::insertOrUpdateRule(mRule &rule,QString &retcode)
{
    NetWorkSendManager::Instance()->saveAlarmRule(rule,retcode);
    QString strBuffer;
    QDateTime time;
    time = QDateTime::currentDateTime();
    strBuffer = time.toString("yyyy-MM-dd hh:mm:ss");

    LOG_INFO("更新规则成功：更新时间:%s,规则id:%d.\n", strBuffer.toStdString().c_str(), rule.rule_id.toStdString().c_str());
}

//删除规则
void dataTranslate::deleteRule(mRule rule,QString &retcode)
{
    NetWorkSendManager::Instance()->deleteAlarmRule(QString("%1").arg(rule.rule_id),retcode);
    QString strBuffer;
    QDateTime time;
    time = QDateTime::currentDateTime();
    strBuffer = time.toString("yyyy-MM-dd hh:mm:ss");    

    LOG_INFO("删除规则成功：删除时间:%s，规则id:%d.\n", strBuffer.toStdString().c_str(), rule.rule_id.toStdString().c_str());
}

//上传报警记录 pictype: 1.png 2.gif
bool dataTranslate::updateAlarmData(const QString &path,mAlarmRecord_stu &alarmrecord,QString &statusCode,int level,QString cameraID,int pictype)
{
    //保存报警记录
    bool ret=false;
    if(pictype==PNG)
    {
        ret=NetWorkSendManager::Instance()->saveAlarmRecord(alarmrecord,statusCode);
    }
    else if(pictype==GIF)
    {
        ret=NetWorkSendManager::Instance()->saveAlarmMP4(path,statusCode,alarmrecord.alarm_id,alarmrecord.rule_id);
    }

    //
    /*
    string levelName;
    QString key=QString("%1_%2").arg(cameraID).arg(alarmrecord.alarm_rule_id);
    if(level==AL_REMIND) levelName="提醒关注";
    else if(level==AL_ALARM) levelName="报警";


    LOG_INFO("上传记录成功or失败：%d,上传时间%s，图片文件%s，报警等级为：%s,触犯规则ID:%s,规则名称：%s,\n",
             ret, alarmrecord.alarmtime.toStdString().c_str(), path.toStdString().c_str(),
             levelName.c_str(), alarmrecord.alarm_rule_id.toStdString().c_str(),
             map_rule[key].rule_name.toStdString().c_str());
*/
    return ret;
}
