#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H
/***************************************************************************
 * //报警规则类，主要根据输入的坐标何规则进行报警判断
 * //输入：坐标集，规则
 * //输出：是否报警
 * //具体功能：（大原则，超过两人都不报警）
 * //1.单人独处报警：在指定区域，规定时间内只有一个人出现在画面中时系统自动报警。摄像头连续N秒(张)，有80%的判断结果显示全部图像范围内仅有一人，前m秒提醒关注，后k秒升级为报警
 * //2.超时滞留：从人进入划定区域开始计时，超过系统设定时间未离开时系统自动报警，可设多个时间段，更长时间未离开时二次报警，并提升报警等级
 * //3.值岗检测：在规定时段，规定区域内的人员进行检测，可以设置值岗人员数量，值岗区域人员数量和时间不满足规则要求时则报警（亦可根据需要设置是否可以连续报警），离岗后再进入不产生报警
 * //4.双人值岗离岗报警：摄像头连续N秒(张), 判断范围內断100%判结果显示图像范围内少于两人，前m秒提醒关注，后k秒升级为报警
 * //5.警戒线/区域报警：在需要保护的区域设置的虚拟线，当目标对象触碰到虚拟线时，触发报警，主要用于监视门/限高及警戒区域范围
 * //6.起身检测：对指定区域指定时间进行监控
 * //7.巡更检测：对民警巡视区域进行检测，当指定时间指定区域内有人则为正常，超过指定时间还未有人进入区域则报警检测：对民警巡视区域进行检测，当指定时间指定区域内有人则为正常，超过指定时间还未有人进入区域则报警
 * //8.视质检测：摄像头遮挡报警：检测到摄像头遮挡，报警，摄像头移位报警：检测到摄像头移位，报警
 * ************************************************************************/


#include <QPoint>
#include "qmyglobal.h"
#include <QRect>

//图像人数
struct tagImageHead
{
    int headNum; //人头数
    int timestamp;//时间戳
};
typedef list<tagImageHead> ImageHeadList;
typedef map<QString, ImageHeadList> ImageHeadMap;//key:cameraID_ruleID
//报警时间
typedef map<QString, int> AlarmStamp;//key cameraID_ruleID
//dealed名
typedef list<QString> ImageNameList;
typedef map<QString, ImageNameList> ImageNameMap;//key cameraID
//alarm discorrect
typedef list<QVector<QPoint>> AlarmDiscorrectList;
typedef map<QString, AlarmDiscorrectList> AlarmDiscorrectMap;//key:cameraID_ruleID

class AlarmManager
{
public:
    static AlarmManager *Instance();

private:
    AlarmManager();

public:
    //是否报警
    void Alarm(const QString &filePath, const QString &cameraID, const tagDetectAll &detectAll);

    //返回mp4路径,filePath:/dealed/1570/1234567.mp4
    bool CreateMp4(const QString &cameraID, const QString &filePath);

    //报警记录
    void PushAlarmRecord(const mAlarmRecord_stu &record);
    bool PopAlarmRecord(mAlarmRecord_stu &record);

    //报警规则
    void SaveRule(const QString &cameraID, const mRule &rule);
    void RemoveRule(const QString &cameraID, const QString &ruleID);
    bool HasRule(const QString &cameraID, QString ruleID);
    bool HasRule(const QString &cameraID, QString ruleID, mRule &rule);
    RuleMap& getRuleMap();

    //不正确的报警
    bool IsAlarmDiscorrect(const QString &key, const QRect &head);
    //head_points:12,34;45,32
    void SaveAlarmDiscorrect(const QString &key, const QVector<QPoint> &vecPoint);
    void ClearAlarmDiscorrect();

private:
    void alarmAnaly(const QString &path, const tagDetectAll &detectAll, const mRule &rule, int nowtime, const QTime &curTime);
    //图像人数
    void saveImageHead(const QString &key, int headNum, int nowtime);
    void getImageHead(const QString &key, int afterTime, int minHeads, int maxHeads, int &imageNum, int &headNum);
    //相机报警时间戳
    int getAlarmStamp(const QString &key);
    void saveAlarmStamp(const QString &key, int timestamp);
    void clearAlarmStamp(const QString &key);
    //mp4，.jpg
    void saveImageName(const QString &cameraID, const QString &imageName);
    void clearImageName(const QString &cameraID);

    //获取key
    QString getRuleKey(const QString &cameraID, const QString &ruleID)
    {
        return QString("%1_%2").arg(cameraID).arg(ruleID);
    }

    //
    //4个点相对区域的位置  返回1：四个点都在区域内 2：四个点都在区域外 3：与外框相交
     int isFourPointInside(const QRect &rects, const QVector<QPoint> &points);
     double percenTage(QMap<int,pendImg> imgs,int num,int state);
     void imgText(QString path,QString str,int w,int h,QColor color);
     int getPersonNum(const QString &key, int alarm_type, int linetype, const QVector<QPoint> &rects, const tagDetectAll &detectAll, QRect &keyHead);
     void drawBox(int percen, const QString &imgPath, const QString &ruleName, const QVector<QPoint> &rulePoints);

private:
     //相机图像人数
     ImageHeadMap m_imageHeads;
     //相机报警时间戳
     AlarmStamp m_alarmStamps;
     QMutex m_mutexAlarm;
     //dealed文件名
     ImageNameMap m_imageNames;
     QMutex m_mutexImage;
     //报警记录
     AlarmRecordList m_listAlarmRecord;
     QMutex m_mutexRecord;
     //规则
     RuleMap m_rules;
     QMutex m_mutexRule;
     //不正确的报警
     AlarmDiscorrectMap m_alarmDiscorrects;
     QMutex m_mutexDiscorrect;

     static AlarmManager* s_alarmManager;
};
#define ALARM_MANAGER AlarmManager::Instance()

#endif // ALARMMANAGER_H
