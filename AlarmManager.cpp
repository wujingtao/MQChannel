#include "AlarmManager.h"
#include <QDateTime>
#include "qdrawline.h"
#include <QPainter>
#include <QProcess>
#include <qjudgeposition.h>
#include <sstream>
using namespace std;

AlarmManager* AlarmManager::s_alarmManager = NULL;

AlarmManager* AlarmManager::Instance()
{
    if(s_alarmManager==NULL)
    {
        s_alarmManager=new AlarmManager;
    }

    return s_alarmManager;
}

AlarmManager::AlarmManager()
{

}

void AlarmManager::Alarm(const QString &filePath, const QString &cameraID, const tagDetectAll &detectAll)
{
    //
    int nowtime=time(NULL);
    QTime currentTime=QTime::currentTime();

    RuleMap::iterator it = m_rules.find(cameraID);
    if(it==m_rules.end())
    {
        return;
    }

    bool firstSave =true;
    RuleList &listRule = it->second;
    for(RuleList::iterator itlist = listRule.begin(); itlist!=listRule.end(); ++itlist)
    {

        QStringList times=itlist->times;
        bool isDeal = false;

        for(int i=0; i<times.size(); i++)
        {
            QString time=times[i];
            QStringList timelist = time.split("-");
            QTime startTime=QTime::fromString(timelist[0],"hh:mm:ss");
            QTime stopTime=QTime::fromString(timelist[1],"hh:mm:ss");
            if(currentTime>startTime&&currentTime<stopTime)
            {
                if(itlist->alarm_type!=6 || detectAll.heads.size()>2)//texuchuli.shuanrenzhigang head need more 2.
                    isDeal=true;

                break;
            }
        }        

        if(isDeal)
        {
            if(firstSave)
            {
                saveImageName(cameraID, filePath);
                firstSave=false;
            }

            alarmAnaly(filePath, detectAll, *itlist, nowtime, currentTime);
        }
        else
        {
            clearAlarmStamp(getRuleKey(cameraID, itlist->rule_id));
        }

    }

    if(firstSave)
    {
        clearImageName(cameraID);
    }
}

void AlarmManager::saveImageName(const QString &cameraID, const QString &imageName)
{
    m_mutexImage.lock();
    ImageNameMap::iterator it = m_imageNames.find(cameraID);
    if(it != m_imageNames.end())
    {
        ImageNameList &listImageName=it->second;
        if(listImageName.size()>60)
        {
            listImageName.pop_front();
        }

        listImageName.push_back(imageName);
    }
    else
    {
        ImageNameList listImageName;
        listImageName.push_back(imageName);

        m_imageNames[cameraID]=listImageName;
    }
    m_mutexImage.unlock();
}

void AlarmManager::clearImageName(const QString &cameraID)
{
    m_mutexImage.lock();
    ImageNameMap::iterator it = m_imageNames.find(cameraID);
    if(it!=m_imageNames.end())
        m_imageNames.erase(it);
    m_mutexImage.unlock();
}

//返回mp4路径,filePath:/dealed/1570/1234567.mp4
bool AlarmManager::CreateMp4(const QString &cameraID, const QString &filePath)
{
    vector<QString> vecMp4Image;

    ImageNameMap::iterator it = m_imageNames.find(cameraID);
    if(it != m_imageNames.end())
    {
        int limit=0;
        ImageNameList &listImageName = it->second;
        for(ImageNameList::reverse_iterator itlist=listImageName.rbegin(); itlist!=listImageName.rend() && limit<MP4_IMAGE_COUNT; ++itlist)
        {
            vecMp4Image.push_back(*itlist);
            ++limit;
        }
    }

    //create a folder for ffmpeg convert pictures to mp4, for it must be like 0.jpg 1.jpg 2.jpg 3.jpg
    QString mp4dir=filePath;
    mp4dir.replace(".mp4","");

    QDir dirmp4(mp4dir);
    if(!dirmp4.exists())
       dirmp4.mkdir(mp4dir);

    int imageNum=vecMp4Image.size();
    for(int i=0;i<imageNum;i++)
    {
        QString newFileName=mp4dir+QString("/%1.jpg").arg(imageNum-i-1);
        QFile::copy(vecMp4Image[i],newFileName);
    }

    QString picsPare=QString("%1/%d.jpg").arg(mp4dir);

    QProcess  process;
    QStringList paramstr;
    //paramstr<<"-delay"<<QString("%1").arg(numPerMinute)<<"-loop"<<"1"<<picsParam<<"-resize"<<size<<outPutPath;
    paramstr<<"-r"<<"1"<<"-f"<<"image2"<<"-i"<<picsPare<<"-vcodec"<<"h264"<<"-y"<<filePath;
    //process.execute(QString("/usr/bin/convert"),paramstr);
    process.execute(QString("/usr/bin/ffmpeg"),paramstr);//command是要执行的命令,args是参数，

    dirmp4.removeRecursively();
    dirmp4.rmdir(mp4dir);

    /*
    if (!process.waitForStarted())
    {
        QString str = QString::fromLocal8Bit(process.readAllStandardError());
        //qDebug()<<"error"<<QString::fromLocal8Bit(process.readAllStandardError());
        LOG_ERROR("createmp4 error:%s.\n", str.toStdString().c_str());
        return false;
    }
    else
    {
        LOG_INFO("create mp4 success.\n");
    }
    */

    return true;
}

void AlarmManager::PushAlarmRecord(const mAlarmRecord_stu &record)
{
    //TODO:大小限制
    m_mutexRecord.lock();
    m_listAlarmRecord.push_back(record);
    m_mutexRecord.unlock();
}

bool AlarmManager::PopAlarmRecord(mAlarmRecord_stu &record)
{
    bool ret=false;
    m_mutexRecord.lock();
    if(!m_listAlarmRecord.empty())
    {
        record = m_listAlarmRecord.front();
        m_listAlarmRecord.pop_front();

        ret=true;
    }
    m_mutexRecord.unlock();

    return ret;
}

void AlarmManager::SaveRule(const QString &cameraID, const mRule &rule)
{
    m_mutexRule.lock();
    RuleMap::iterator it = m_rules.find(cameraID);

    if(it != m_rules.end())
    {
        it->second.push_back(rule);
    }
    else
    {
        RuleList listRule;
        listRule.push_back(rule);

        m_rules[cameraID]=listRule;
    }
    m_mutexRule.unlock();
}

void AlarmManager::RemoveRule(const QString &cameraID, const QString &ruleID)
{
    m_mutexRule.lock();
    RuleMap::iterator it = m_rules.find(cameraID);
    if(it != m_rules.end())
    {
        RuleList &listRule = it->second;
        for(RuleList::iterator itlist = listRule.begin(); itlist!=listRule.end(); ++itlist)
        {
            if(itlist->rule_id.compare(ruleID)==0)
            {
                listRule.erase(itlist);
                break;
            }
        }
    }
    m_mutexRule.unlock();
}

bool AlarmManager::HasRule(const QString &cameraID, QString ruleID)
{
    RuleMap::iterator it = m_rules.find(cameraID);
    if(it != m_rules.end())
    {
        RuleList &listRule = it->second;
        for(RuleList::iterator itlist = listRule.begin(); itlist!=listRule.end(); ++itlist)
        {
            if(itlist->rule_id.compare(ruleID)==0)
            {
                return true;
            }
        }
    }

    return false;
}

bool AlarmManager::HasRule(const QString &cameraID, QString ruleID, mRule &rule)
{

    RuleMap::iterator it = m_rules.find(cameraID);
    if(it != m_rules.end())
    {
        RuleList &listRule = it->second;
        for(RuleList::iterator itlist = listRule.begin(); itlist!=listRule.end(); ++itlist)
        {
            if(itlist->rule_id.compare(ruleID)==0)
            {
                rule = *itlist;
                return true;
            }
        }
    }

    return false;
}

RuleMap& AlarmManager::getRuleMap()
{
    return m_rules;
}

bool AlarmManager::IsAlarmDiscorrect(const QString &key, const QRect &head)
{
    AlarmDiscorrectMap::iterator it = m_alarmDiscorrects.find(key);
    if(it!=m_alarmDiscorrects.end())
    {
        AlarmDiscorrectList &listAlarmDiscorrect = it->second;
        for(AlarmDiscorrectList::iterator itlist = listAlarmDiscorrect.begin(); itlist!=listAlarmDiscorrect.end(); ++itlist)
        {
            int ret=isFourPointInside(head, *itlist);
            if(ret==1 || ret==3)
            {
                return true;
            }
        }
    }

    return false;
}

void AlarmManager:: SaveAlarmDiscorrect(const QString &key, const QVector<QPoint> &vecPoint)
{
    //TODO:最好限制10个
    m_mutexDiscorrect.lock();
    AlarmDiscorrectMap::iterator it = m_alarmDiscorrects.find(key);
    if(it!=m_alarmDiscorrects.end())
    {
        m_alarmDiscorrects[key].push_back(vecPoint);
    }
    else
    {
        AlarmDiscorrectList listAlarmDiscorrect;
        listAlarmDiscorrect.push_back(vecPoint);

        m_alarmDiscorrects[key]=listAlarmDiscorrect;
    }
    m_mutexDiscorrect.unlock();
}

void AlarmManager::ClearAlarmDiscorrect()
{
    m_mutexDiscorrect.lock();
    m_alarmDiscorrects.clear();
    m_mutexDiscorrect.unlock();
}

void AlarmManager::alarmAnaly(const QString &path, const tagDetectAll &detectAll, const mRule &rule, int nowtime, const QTime &curTime)
{
    QString key = getRuleKey(rule.camera_id, rule.rule_id);
    int alarmtime = getAlarmStamp(key);

    int nextAlarmTime = rule.two_remind_time+alarmtime;
    if(alarmtime==0)
    {
        nextAlarmTime=nowtime;
        saveAlarmStamp(key, nowtime);
    }

    //未到二次提醒时间
    /*
    if(nowtime < nextAlarmTime)
    {
        return;
    }
    */
    int analyAlarmTime = nowtime - rule.alarm_delay;

    //报警延时内图片人数
    QRect keyHead;//danrenduchu head point
    int personNum = getPersonNum(key, rule.alarm_type, rule.line_type, rule.points, detectAll, keyHead);
    saveImageHead(key, personNum, nowtime);

    int imageNum=0;
    int headNum=0;
    getImageHead(key, analyAlarmTime, rule.alarm_person_num, rule.max_alarm_person_num, imageNum, headNum);


    int percent = 0;
    if(imageNum!=0)
    {
        percent = headNum*100/imageNum;
    }


    //haoshi 25ms
    drawBox(percent, path, rule.rule_name, rule.points);

    if(percent >= rule.alarm_threshold && nowtime >= (nextAlarmTime+rule.alarm_delay))
    {
        //上传报警
        mAlarmRecord_stu record;
        record.cameraID=rule.camera_id;
        record.rule_id = rule.rule_id;
        record.alarm_id = "";
        record.alarmLevel=AL_REMIND;
        if(rule.alarm_delay > 0)
        {
            record.alarmLevel=AL_ALARM;
        }
        record.flag=1;

        record.filePath = path;
        record.alarmtime = curTime.toString("hh:mm:ss");
        record.head_points = QString("%1,%2,%3,%4").arg(keyHead.x()).arg(keyHead.y()).arg(keyHead.width()).arg(keyHead.height());

        PushAlarmRecord(record);
        saveAlarmStamp(key, nowtime);


            //LOG_INFO("alarm ruleid:%d, time:%s\n", rule.rule_id.toStdString().c_str(), record.alarmtime.toStdString().c_str());
        }

}

void AlarmManager::saveImageHead(const QString &key, int headNum, int nowtime)
{
    tagImageHead imageHead;
    imageHead.headNum=headNum;
    imageHead.timestamp=nowtime;

    ImageHeadMap::iterator it = m_imageHeads.find(key);
    if(it != m_imageHeads.end())
    {
        if(it->second.size() > IMAGE_HEAD_COUNT)
        {
            it->second.pop_front();
        }
        it->second.push_back(imageHead);
    }
    else
    {
        ImageHeadList listImageHead;
        listImageHead.push_back(imageHead);

        m_imageHeads[key]  = listImageHead;
    }
}

void AlarmManager::getImageHead(const QString &key, int afterTime, int minHeads, int maxHeads, int &imageNum, int &headNum)
{
    ImageHeadMap::iterator it = m_imageHeads.find(key);
    if(it != m_imageHeads.end())
    {
        ImageHeadList &listImageHead = it->second;
        for(ImageHeadList::reverse_iterator itlist = listImageHead.rbegin(); itlist != listImageHead.rend(); ++itlist)
        {
            if(itlist->timestamp < afterTime)
            {
                break;
            }

            if(itlist->headNum >= minHeads && itlist->headNum <= maxHeads)
            {
                ++headNum;
            }

            ++imageNum;
        }
    }
}

//获取相机报警时间
int AlarmManager::getAlarmStamp(const QString &key)
{
    int stamp=0;
    m_mutexAlarm.lock();
    AlarmStamp::iterator it = m_alarmStamps.find(key);
    if(it != m_alarmStamps.end())
    {
        stamp = it->second;
    }
    m_mutexAlarm.unlock();

    return stamp;
}

void AlarmManager::saveAlarmStamp(const QString &key, int timestamp)
{
    m_mutexAlarm.lock();
    m_alarmStamps[key] = timestamp;
    m_mutexAlarm.unlock();
}

void AlarmManager::clearAlarmStamp(const QString &key)
{
    m_mutexAlarm.lock();
    AlarmStamp::iterator it = m_alarmStamps.find(key);
    if(it!=m_alarmStamps.end())
        m_alarmStamps.erase(it);
    m_mutexAlarm.unlock();
}

//4个点相对区域的位置  返回1：四个点都在区域内 2：四个点都在区域外 3：与外框相交
int AlarmManager::isFourPointInside(const QRect &head, const QVector<QPoint> &vecRule)
{
    QPoint leftUpPoint(head.x(),head.y());
    QPoint leftDownPoint(head.x(), head.y()+head.height());
    QPoint rightUpPoint(head.x()+head.width(), head.y());
    QPoint rightDownPoint(head.x()+head.width(), head.y()+head.height());
    bool isleftUpPointIn=QJudgePosition::insidepolygon(leftUpPoint, vecRule);
    bool isleftDownPointIn=QJudgePosition::insidepolygon(leftDownPoint, vecRule);
    bool isrightUpPointIn=QJudgePosition::insidepolygon(rightUpPoint, vecRule);
    bool isrightDownPointIn=QJudgePosition::insidepolygon(rightDownPoint, vecRule);
    if(isleftUpPointIn&&isleftDownPointIn&&isrightUpPointIn&&isrightDownPointIn)
    {
        return 1;
    }
    else if(!isleftUpPointIn&&!isleftDownPointIn&&!isrightUpPointIn&&!isrightDownPointIn)
    {
        return 2;
    }
    else return 3;
}

int  AlarmManager::getPersonNum(const QString &key, int alarm_type, int linetype, const QVector<QPoint> &rects, const tagDetectAll &detectAll, QRect &keyHead)
{
    if(rects.size()==0) return 0;

    QVector<tagCaffeObject> vecCaffeObj=detectAll.heads;
    if(linetype==3)
    {
        vecCaffeObj=detectAll.bodys;
    }

    int num=0;    
    for(int i=0;i< vecCaffeObj.size();i++)
    {
        //是否單人獨處誤報
        if(alarm_type==1 && IsAlarmDiscorrect(key, vecCaffeObj[i].rect)) continue;

        int ret=isFourPointInside(vecCaffeObj[i].rect, rects);

        if(linetype==1)
        {
            if(ret==2 || ret==3) ++num;
        }
        else
        {
            if(ret==1 || ret==3)
            {
                ++num;

                if(alarm_type==1)
                    keyHead=vecCaffeObj[i].rect;
            }
        }
    }

    return num;
}

double AlarmManager::percenTage(QMap<int,pendImg> imgs,int minPerson,int maxPerson)
{
    double count= 0;
    int imgNum =1;
    double percen = 0;
    imgNum = imgs.size();
    QMap<int,pendImg>::iterator iter;
    for(iter=imgs.begin();iter!=imgs.end();iter++)
    {
         pendImg img =iter.value();
         int personCount=img.personNum;
         if(personCount >= minPerson &&personCount < maxPerson)
         {
           // if(img.hasBody)
            count=count+1;
//            else
//            count=count+0.5;
         }
    }
    percen = (double)count/(double)imgNum;
    return percen;
}

void AlarmManager::imgText(QString path,QString str,int w,int h,QColor color)
{
    QImage image(path);//=QPixmap(path).toImage();//绝对路径
       //以你原来的那张图片来
       //构造一个image
       //qDebug()<<image.width();

       QPainter painter(&image); //为这个QImage构造一个QPainter
       painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
       //设置画刷的组合模式CompositionMode_SourceOut这个模式为目标图像在上。
       // color = Qt::blue;
       //改变画笔和字体
       QPen pen = painter.pen();
       pen.setColor(color);
       QFont font = painter.font();
       font.setBold(false);//加粗
       font.setPixelSize(20);//改变字体大小

       painter.setPen(pen);
       painter.setFont(font);

       painter.drawText(w,h,str);

       image.save(path,0);
}

void AlarmManager::drawBox(int percen,const QString &imgPath, const QString &ruleName, const QVector<QPoint> &rulePoints)
{
/*
    255 160 122
    255 165 0
    240 128 128
    255 20 147
    255 0 0
*/

    QString str = QString("%1:%2%").arg(ruleName).arg(percen);

    QColor color(0,255,0);
    if(10 < percen&& percen<= 20)
    {
         color.setRgb(255,210,210);
    }
    else if(20 < percen&& percen <= 40)
    {
         color.setRgb(255,180,180);
    }
    else if(40<percen&& percen<=60)
    {
        color.setRgb(255,120,120);
    }
    else if(60<percen&& percen<=80)
    {
        color.setRgb(255,60,60);
    }
    else if(80<percen&& percen<=100)
    {
        color.setRgb(255,0,0);
    }

    QImage image(imgPath);//=QPixmap(path).toImage();//绝对路径

    QPainter painter(&image); //为这个QImage构造一个QPainter
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    //设置画刷的组合模式CompositionMode_SourceOut这个模式为目标图像在上。
    //1.
    //改变画笔和字体
    QPen pen = painter.pen();
    pen.setColor(color);
    pen.setWidth(2);
    QFont font = painter.font();
    font.setBold(false);//加粗
    font.setPixelSize(20);//改变字体大小

    painter.setPen(pen);
    painter.setFont(font);

    painter.drawText(rulePoints[0].x(), rulePoints[0].y(), str);

    //2.
    QPoint p1, p2;
    for(int i=0; i<rulePoints.size(); i++)
    {
        if(i==0)
        {
            p1=rulePoints[0];
            continue;
        }
        p2=rulePoints[i];

        painter.drawLine(p1, p2);
        p1=p2;
    }
    painter.drawLine(p1, rulePoints[0]);

    image.save(imgPath);
}
//
