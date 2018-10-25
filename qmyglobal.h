        #ifndef QMYGLOBAL_H
    #define QMYGLOBAL_H
    #include <QObject>
    #include <QWidget>
    #include<QPaintEvent>
    #include<QEvent>
    #include<QPushButton>
    #include<QMap>
    #include <QMutex>
    #include "qlogfile.h"
    #include "param.h"
    #include "Logger.h"
    #include <list>
    #include <vector>
    #include <fstream>
    using namespace std;

//cv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

    extern int screenX;
    extern int screenY;
    extern QString userName;
    extern QString passWD;
    extern QString session_id;
    //extern QLogFile *g_pLogFile;
    using namespace std;



    typedef struct struct_Machine
    {
        QString MachineId;         //区域关联编号
        QString id;             //区域自身ID
        QString MachineName;   //区域名称
        QString MachineAdress; //区域地址
    } mMachine;

    typedef struct struct_Area
    {
        QString AreaId;         //区域关联编号
        QString id;             //区域自身ID
        QString AreaName;   //区域名称
        QString AreaAdress; //区域地址
    } mArea;

    //报警得分和框
    typedef struct detection_all{
        QVector<QRect> nodelist;
        QVector<float> scores;
    }detect_all;

    struct tagCaffeObject
    {
        QRect rect;
        float score;
    };
    struct tagDetectAll
    {
        QVector<tagCaffeObject> heads;
        QVector<tagCaffeObject> bodys;
        QVector<tagCaffeObject> polices;
    };

    //字典
    typedef struct struct_dictionary
    {
        int         id;   //id
        QString     scope;
        QString     update_time;
        QString     remark;    //说明
        QString     name;
        int         darea_id;
        QString     create_time; //创建时间
        QString     code;
        QString     type;
        int         dstatus;     //启用状态
        QString     oper_id;
    }mDictionary;

    //报警规则
    typedef struct struct_rule
    {
        QString          rule_id;   //规则id，数据库生成的18位long
        int         alarm_type;//报警类型  1:单人独处(自动生成，不包含点集) 2.超时滞留 3.值岗检测 4.双人值岗 5.警戒线 6.虚拟墙 7.起身检测 8.巡更检测 9.视质检测(自动生成，不包含点集)
        QString     remark;    //说明
        QString         camera_id; //摄像头ID
        int         line_type; //画线类型  1.从区域内进入区域外报警  2.从区域外进入区域内报警 3. 从线左到线右报警 4. 从线右到线左报警  5.从线上方到线下方  6.从线下方到线上方
        int         alarm_person_num;//最少报警人数
        QString     update_time; //更新时间
        int     alarm_delay; //报警延时(0lijibaojing)
        int     alert_delay; //关注延时(0lijiguangzhu)
        int     max_alarm_person_num;//最多报警人数
        int  alarm_threshold;//报警阈值
        int  alert_threshold;//关注阈值
        int      two_remind_time;//二次提醒时间
        QStringList     times;       //报警时间段集
        QString     create_time; //报警创建时间
        QVector<QPoint>     points;      //点坐标集
        int     dstatus;     //启用状态
        QString     rule_name;   //规则名称
        QString     oper_id;
    }mRule;


    //报警记录
    typedef struct _alarmrecord_stu
    {
        QString remain_pic_path;       //图片路径
        QString alarm_pic_path;       //图片路径
        QString base64_pic;     //base64的图片
        QString alarmtime;      //报警时间,需要
        QString rule_id;  //报警规则id，需要
        QString alarm_id;       //后台保存的报警记录ID

        int    RemindFlag;       //是否提醒关注  0: 不提醒  1： 提醒未上传  2：提醒已上传
        int    AlarmFlag;        //是否报警    0: 不报警  1： 报警未上传  2：报警已上传
        int    cancelTimes;      //连续多次判断到则取消报警
        bool   mp4upload;
        bool   closeAlarm;

        QString cameraID;
        int alarmLevel;
        QString filePath; //img路径
        int flag; //0.默认 1.上传img 2.上传mp4
        QString head_points;

    }mAlarmRecord_stu;
    typedef list<mAlarmRecord_stu> AlarmRecordList;
    typedef QMap<QString ,mAlarmRecord_stu> AlarmRecordMap;

    //待处理图片
    typedef struct struct_img
    {
        QString imgPath;//图片路径
        QVector<QRect> rect;//人头区域
        int personNum;
        bool hasBody;  //是否有身体
    }pendImg;

    typedef struct struct_uploadData
    {
         QString upPath;
         QMap<QString, mAlarmRecord_stu> upAlarmRecord;
         QString upCameraId;
    }stru_uploadData;

    typedef list<mRule> RuleList;
    typedef map<QString, RuleList> RuleMap;//key:cameraID

    enum Message_Level{NORMALINFO=0,ERRORINFO};
    enum AlarmPicType{PNG=1,GIF};
    enum AlarmLevel{AL_REMIND=1, AL_ALARM};
    extern QMap<int,mDictionary> map_AlarmType; //报警类型队列
    extern QMap<int,mDictionary> map_LineType; //画线类型队列
    //extern QMap<QString,mMachine> map_Machine; //存储从上端获取的服务器队列
    extern QMap<QString,mArea> map_Area; //存储从上端获取的区域队列,默认一条且不关联(备注：数据库相机的area_id值为server_id+_+channel_id)
    extern RuleMap g_rules;
    extern QMap<QString,mRule> map_CutImageRule; //单独存储摄像头的有效范围队列
    extern QMutex caffe_mutex;
    extern QMutex caffe_mutex2;
    extern QMutex caffe_mutex3;
    //TODO:del
    extern QMutex mapAfterDeal_mutex;
    extern QMutex mapAfterDeal_mutex2;
    extern QMutex mapAfterDeal_mutex3;
    extern QMutex caffeDeal_mutex;
    extern bool exitapp;
    extern QString g_IP;
    extern QString g_Port;
    extern QString g_AnalyseServerID;
    extern QString g_Server_Adress;
    extern double window_width_video;
    extern double window_height_video;
    extern void delaymsec(int ms);
    extern void WriteSettings(QString username,QString passwd,QString serverIP ,QString serverPort,QString serverID);
    extern void ReadSettings(QString &username,QString &passwd,QString &serverIP ,QString &serverPort,QString &serverID);
    extern bool islogin;



    extern QMutex upMutex;
    extern QMutex afterupMutex;
    extern QList<stru_uploadData> list_upload;
    extern QList<stru_uploadData> list_afterUpload;
    class qmyglobal
    {
    public:
        qmyglobal();
    };

    class main3Button:public QPushButton//用于四态图片
    {
    public:
        main3Button(QString pix_listurl,QWidget*parent);
    protected:
        void paintEvent(QPaintEvent *);
        void enterEvent(QEvent *);
        void leaveEvent(QEvent*);
        void mousePressEvent(QMouseEvent *e);
        void mouseReleaseEvent(QMouseEvent *e) override;
    private:
        QList<QPixmap> m_pixlist;
        int m_index;
        bool m_enter;
    };

    extern cv::Mat QImageToMat(QImage image);
    extern QImage MatToQImage(const cv::Mat& mat);

    #endif // QMYGLOBAL_H
