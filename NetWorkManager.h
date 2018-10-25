#ifndef NETWORKSENDMANAGER_H
#define NETWORKSENDMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QVector>
#include "qmyglobal.h"
#if _DEBUG
#define INTERFACEURL "http://192.168.1.108:8080"
#else
#define INTERFACEURL "http://192.168.1.108:8080"
#endif

typedef struct _rcv_stu
{
    int         status;
    QString     msg;
    QJsonObject data;
}rcv_stu;

class HttpRequest;
class NetWorkSendManager : public QObject
{
public:
    static NetWorkSendManager* Instance();

    bool getLogin(QString userName,QString password,QString &retCode);
    bool getCameraList(QString server_id,QString &retCode); //查询摄像头信息
    bool getCameraRTSPList();//查询摄像头RTSP地址信息
    bool getDictionaryList(QString dictID,QString &retCode);//查询字典列表   报警规则；画线规则
    bool getAlarmRule(QString cameraID,QString &retCode);//查询报警规则
    bool saveAlarmRule(mRule &data,QString &retCode);//保存报警规则
    bool deleteAlarmRule(QString ruleID,QString &retCode);//删除报警规则
    bool saveAlarmRecord(mAlarmRecord_stu &data,QString &statusCode);//保存报警记录
    bool saveAlarmMP4(QString filename,QString &retCode,QString alarm_id,QString alarm_rule_id);

    //有不正确报警
    bool HasAlarmDiscorrect(int &maxID, int &alarmDiscorrectCount);
    //获取不正确报警
    bool GetAlarmDiscorrect();

private:
	NetWorkSendManager(QObject  *parent =  nullptr);
	~NetWorkSendManager();
};
#define NETWORK_MANAGER NetWorkSendManager::Instance()

#endif
