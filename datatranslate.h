#ifndef DATATRANSLATE_H
#define DATATRANSLATE_H
/***************************************************************************
 * //和服务端的数据交互类
 * ************************************************************************/
#include <QString>
#include "qmyglobal.h"
#include "NetWorkManager.h"
class dataTranslate
{
public:
    static dataTranslate* Instance()
    {
        if(s_translate==NULL)
        {
            s_translate=new dataTranslate;
        }

        return s_translate;
    }

public:
    //登陆
    bool login(QString &retcode);
    //获取摄像头配置
    void getCameraConfig(QString str,QString &retcode);
    //获取摄像头规则
    void getCameraRule(QString str,QString &retcode);
    //获取摄像头RTSP
    void getCameraRTSP(QStringList cameraIdlist,QString &retcode);
    //获取画线类型
    void getLineType(QString &retcode);
    //获取告警类型
    void getAlarmType(QString &retcode);
    //插入新规则或更新旧规则
    void insertOrUpdateRule(mRule &rule,QString &retcode);
    //删除规则
    void deleteRule(mRule rule,QString &retcode);

    //上传报警记录
    bool updateAlarmData(const QString &path,mAlarmRecord_stu &alarmrecord,QString &statusCode,int level,QString m_cameraID,int pictype);

private:
    dataTranslate();

public:
    static dataTranslate *s_translate;
};
#define DATA_TRANSLATE dataTranslate::Instance()

#endif // DATATRANSLATE_H
