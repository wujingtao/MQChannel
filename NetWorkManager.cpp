#include "NetWorkManager.h"
#include "HttpRequest.h"
#include <QDebug>
#include "AlarmManager.h"
#include "GlobalData.h"

NetWorkSendManager* NetWorkSendManager::Instance()
{
    static NetWorkSendManager  ins;
    return &ins;
}

NetWorkSendManager::NetWorkSendManager(QObject *parent)
    : QObject(parent)
{

}

NetWorkSendManager::~NetWorkSendManager()
{

}
//isDeviceFt 1

bool NetWorkSendManager::getLogin(QString username,QString password,QString &retCode) //登陆
{
    bool bRet = false;
    QJsonObject mainjson;
    mainjson.insert("org_no", "3");
    mainjson.insert("access_key", "");
    mainjson.insert("userName", username);
    mainjson.insert("password", password);

    QJsonDocument document;
    document.setObject(mainjson);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);

    HttpRequest httpreq;
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/login/login");
    qDebug()<<url;
    QString sessionID="";
    QString status_code="0";
    QString rcvstr = httpreq.HttpSynPost(url,QString(byte_array),sessionID,status_code);
    if(status_code!="200"&&status_code!="302")
    {
        retCode=status_code;
        bRet=false;
        return bRet;
    }
    session_id=sessionID;
    if(session_id.isEmpty())
    {
        bRet=false;
        return bRet;
    }

    bRet=true;
    if (!rcvstr.isEmpty())
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);

        if (json_error.error == QJsonParseError::NoError)
        {
        }
    }
    return bRet;
}

bool NetWorkSendManager::getAlarmRule(QString cameraID,QString &retCode)//查询报警规则
{
    bool bRet = false;
    QJsonObject mainjson;
    mainjson.insert("org_no", "3");
    mainjson.insert("access_key", "");
    mainjson.insert("camera_id", cameraID);

    QJsonDocument document;
    document.setObject(mainjson);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    HttpRequest httpreq;
    QString status_code="0";
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/alarm/listAlarmCameraRule");
    QString rcvstr = httpreq.HttpSynPost(url,QString(byte_array),session_id,status_code);
    qDebug()<<rcvstr;
    if(status_code!="200")
    {
        retCode=status_code;
        bRet=false;
        return bRet;
    }
    if (!rcvstr.isEmpty())
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);

        if (json_error.error == QJsonParseError::NoError)
        {
            if (parse_doucment.isObject())
            {
                QJsonValue arrays_value;
                QJsonValue valuetemp  = parse_doucment.object();
                if(valuetemp.isObject())
                {
                    QJsonObject result_info_obj = valuetemp.toObject();
                    if(result_info_obj.contains("datas"))
                    {
                        arrays_value=result_info_obj.value("datas");
                    }

                }

                if (arrays_value.isArray())//判断他是不是json数组
                {
                    QJsonArray dataArray = arrays_value.toArray();;
                    for (int i = 0; i < dataArray.size(); i++)
                    {
                        QJsonValue dataArrayValue = dataArray.at(i);
                        if(dataArrayValue.isObject())
                        {
                            mRule rule;
                            QJsonObject obj = dataArrayValue.toObject();
                            if(obj.contains("rule_id")&&!obj.value("rule_id").isNull())
                                rule.rule_id = obj.value("rule_id").toString();
                            if(obj.contains("alarm_type")&&!obj.value("alarm_type").isNull())
                                rule.alarm_type = obj.value("alarm_type").toInt();
                            if(obj.contains("remark")&&!obj.value("remark").isNull())
                                rule.remark = obj.value("remark").toString();
                            if(obj.contains("camera_id")&&!obj.value("camera_id").isNull())
                                rule.camera_id = obj.value("camera_id").toString();
                            if(obj.contains("line_type")&&!obj.value("line_type").isNull())
                                rule.line_type = obj.value("line_type").toInt();
                            if(obj.contains("min_alarm_person_num")&&!obj.value("min_alarm_person_num").isNull())
                                rule.alarm_person_num = obj.value("min_alarm_person_num").toInt();
                            if(obj.contains("update_time")&&!obj.value("update_time").isNull())
                                rule.update_time = obj.value("update_time").toString();
                            if(obj.contains("alarm_delay")&&!obj.value("alarm_delay").isNull())
                                rule.alarm_delay = obj.value("alarm_delay").toInt();
                            if(obj.contains("alert_delay")&&!obj.value("alert_delay").isNull())
                                rule.alert_delay = obj.value("alert_delay").toInt();
                            if(obj.contains("max_alarm_person_num")&&!obj.value("max_alarm_person_num").isNull())
                                rule.max_alarm_person_num = obj.value("max_alarm_person_num").toInt();
                            if(obj.contains("alarm_threshold")&&!obj.value("alarm_threshold").isNull())
                                rule.alarm_threshold = obj.value("alarm_threshold").toString().toInt();
                            if(obj.contains("alert_threshold")&&!obj.value("alert_threshold").isNull())
                                rule.alert_threshold = obj.value("alert_threshold").toString().toInt();
                            if(obj.contains("two_remind_time")&&!obj.value("two_remind_time").isNull())
                                rule.two_remind_time = obj.value("two_remind_time").toInt();
                            if(obj.contains("times")&&!obj.value("times").isNull())
                            {
                                QString strtimes = obj.value("times").toString();
                                strtimes.replace(")","");
                                strtimes.replace("(","");
                                rule.times=strtimes.split(",");
                            }
                            if(obj.contains("create_time")&&!obj.value("create_time").isNull())
                                rule.create_time = obj.value("create_time").toString();
                            if(obj.contains("points")&&!obj.value("points").isNull())
                            {
                                QString str = obj.value("points").toString();
                                QStringList strlist=str.split("),(");
                                for(int i=0;i<strlist.size();i++)
                                {
                                    QString pointstr=strlist[i];
                                    pointstr.replace(")","");
                                    pointstr.replace("(","");
                                    QStringList pointx_y=pointstr.split(",");
                                    if(pointx_y.size()==2)
                                    {
                                        QPoint point;
                                        point.setX(pointx_y[0].toInt());
                                        point.setY(pointx_y[1].toInt());
                                        rule.points.append(point);
                                    }
                                }
                            }
                            if(obj.contains("dstatus")&&!obj.value("dstatus").isNull())
                                rule.dstatus = obj.value("dstatus").toInt();
                            if(obj.contains("rule_name")&&!obj.value("rule_name").isNull())
                                rule.rule_name = obj.value("rule_name").toString();
                            if(obj.contains("oper_id")&&!obj.value("oper_id").isNull())
                                rule.oper_id = obj.value("oper_id").toInt();

                            ALARM_MANAGER->SaveRule(rule.camera_id, rule);

                            //增加截图区域队列，id好顺延，有跨度可能有bug
                            if(rule.alarm_type==10)
                            {
                                map_CutImageRule.insert(rule.camera_id,rule);
                            }
                        }
                    }
                }
                bRet = true;
            }
            else
            {
                bRet = false;
            }
        }
    }
    return bRet;
}

bool NetWorkSendManager::getCameraList(QString serve_id,QString &retCode) //查询摄像头信息
{
    bool bRet = false;
    QJsonObject mainjson;
    mainjson.insert("org_no", "3");
    mainjson.insert("access_key", "");
    mainjson.insert("server_id", serve_id);

    QJsonDocument document;
    document.setObject(mainjson);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);

    HttpRequest httpreq;
    QString status_code="0";
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/alarm/listAlarmCameraServerCfg");
    QString rcvstr = httpreq.HttpSynPost(url,QString(byte_array),session_id,status_code);
    if(status_code!="200")
    {
        retCode=status_code;
        bRet=false;
        return bRet;
    }
    //{\"result_code\":\"0\",\"result_info\":{\"datas\":[{\"d_id\":3,\"text\":\"监控\",\"pid\":1},{\"d_id\":4,\"text\":\"监控\",\"pid\":2},{\"id\":1,\"text\":\"一监区\",\"pid\":3},{\"id\":2,\"text\":\"一监室\",\"pid\":3},{\"id\":3,\"text\":\"二监室\",\"pid\":3},{\"id\":4,\"text\":\"二监区\",\"pid\":3}]}}
    if (!rcvstr.isEmpty())
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);

        if (json_error.error == QJsonParseError::NoError)
        {
            if (parse_doucment.isObject())
            {
                QJsonValue arrays_value;
                QJsonValue valuetemp  = parse_doucment.object();
                if(valuetemp.isObject())
                {
                    QJsonObject result_info_obj = valuetemp.toObject();
                    if(result_info_obj.contains("datas"))
                    {
                        arrays_value=result_info_obj.value("datas");
                    }

                }

                if (arrays_value.isArray())//判断他是不是json数组
                {
                    QJsonArray dataArray = arrays_value.toArray();;
                    for (int i = 0; i < dataArray.size(); i++)
                    {
                        QJsonValue dataArrayValue = dataArray.at(i);
                        if(dataArrayValue.isObject())
                        {
                            QString text;
                            QString id;
                            QString pid;
                            QJsonObject obj = dataArrayValue.toObject();
                            if(obj.contains("text")&&!obj.value("text").isNull())
                                text = obj.value("text").toString();
                            if(obj.contains("pid")&&!obj.value("pid").isNull())
                                pid = obj.value("pid").toString();//serverID+"_"+channelID
                            if(obj.contains("pid")&&!obj.value("pid").isNull())
                                id = obj.value("device_id").toString();

                            if(pid != '0' && pid != '-1')
                            {
                                if(id!="-1" && id!="")//web生成了一条默认空数据
                                {
                                    GLOBAL_DATA->SaveCamera(id);
                                }
                            }
                            else if(pid == '0' && pid != '-1')
                            {
                                //if(id!="-1" && id!="")
                                {
                                mArea area;
                                area.AreaName=text;
                                area.AreaId=pid;
                                area.id=id;
                                map_Area.insert(id,area);
                                }
                            }
                        }
                    }
                }
                bRet = true;
            }
            else
            {
                bRet = false;
            }
        }
    }
    return bRet;
}

bool NetWorkSendManager::getCameraRTSPList() //查询摄像头RTSP信息
{
    QJsonObject mainjson;
    mainjson.insert("org_no", "3");
    mainjson.insert("access_key", "");

    QJsonArray mainjson_device;
    vector<QString> vecCamera = GLOBAL_DATA->GetCamera();
    for(size_t i=0; i<vecCamera.size(); i++)
    {
        mainjson_device.append(vecCamera[i]);
    }

    mainjson.insert("device_id", mainjson_device);
    mainjson.insert("isDeviceFt",1);

    QJsonDocument document;
    document.setObject(mainjson);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    qDebug()<<byte_array;
    HttpRequest httpreq;
    QString status_code="0";
    //QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/device/listDeviceView");
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/alarm/listDeviceView");
    QString rcvstr = httpreq.HttpSynPost(url,QString(byte_array),session_id,status_code);

    if(status_code!="200")
    {
        LOG_ERROR("status_code!=200");
        return false;
    }

    if(rcvstr.isEmpty())
    {
        LOG_ERROR("recvstr isEmpty");
        return false;
    }

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);
    if (json_error.error != QJsonParseError::NoError)
    {
        LOG_ERROR("json_error");
        return false;
    }

    if (!parse_doucment.isObject())
    {
        LOG_ERROR("parse_doucment not isObject");
        return false;
    }

    QJsonValue arrays_value;
    QJsonValue valuetemp  = parse_doucment.object();
    if(valuetemp.isObject())
    {
        QJsonObject result_info_obj = valuetemp.toObject();
        if(result_info_obj.contains("data"))
        {
            arrays_value=result_info_obj.value("data");
        }

        if (!arrays_value.isArray())//判断他是不是json数组
        {
            LOG_ERROR("arrays_value not isArray");
            return false;
        }

        QJsonArray dataArray = arrays_value.toArray();
        for (int i = 0; i < dataArray.size(); i++)
        {
            QJsonValue dataArrayValue = dataArray.at(i);
            if(dataArrayValue.isObject())
            {
                QString device_name;
                QString rtsp;
                QString camera_id;
                QString channel_id;
                QString num_id;
                QString stay_time;
                QJsonObject obj = dataArrayValue.toObject();

                if(obj.contains("camera_id")&&!obj.value("camera_id").isNull())
                    camera_id = obj.value("camera_id").toString();
                if(obj.contains("rtsp")&&!obj.value("rtsp").isNull())
                    rtsp=obj.value("rtsp").toString();
                if(obj.contains("camera_name")&&!obj.value("camera_name").isNull())
                    device_name=obj.value("camera_name").toString();
                if(obj.contains("channel_id")&&!obj.value("channel_id").isNull())
                    channel_id=obj.value("channel_id").toString();
                if(obj.contains("num_id")&&!obj.value("num_id").isNull())
                    num_id=obj.value("num_id").toString();
                if(obj.contains("stay_time")&&!obj.value("stay_time").isNull())
                    stay_time=obj.value("stay_time").toString();

                if(GLOBAL_DATA->hasCamera(camera_id))
                {
                    mIPC ipc;
                    ipc.IPCAdress=rtsp;
                    ipc.IPCId=camera_id;
                    ipc.IPCName=device_name;
                    ipc.channel_id=channel_id.toInt();
                    ipc.num_id=num_id.toInt();
                    ipc.stay_time=stay_time;                 

                    if(stay_time.contains("minutes:"))
                    {
                        QString temptime=stay_time;
                        ipc.minutes=temptime.replace("minutes:", "").toInt();
                    }
                    else if(stay_time.contains("times"))
                    {
                        QString temptime=stay_time;
                        QStringList timelist = temptime.replace("times:","").split("-");
                        if(timelist.size()>=2)
                        {
                            QString st = timelist[0]+":00";
                            QString et = timelist[1]+":59";
                            ipc.minutes=0;
                            ipc.start_time = QTime::fromString(st.replace(" ", ""), "hh:mm:ss");
                            ipc.end_time = QTime::fromString(et.replace(" ", ""), "hh:mm:ss");
                        }
                    }
                    else
                    {
                        ipc.minutes = 0;
                        ipc.start_time = QTime::fromString("00:00:00", "hh:mm:ss");
                        ipc.end_time = QTime::fromString("23:59:59", "hh:mm:ss");
                    }

                    GLOBAL_DATA->SaveChannel(ipc.channel_id, ipc);
                }
            }
        }
    }

    return true;
}

bool NetWorkSendManager::getDictionaryList(QString dictType,QString &retCode)//查询字典列表   报警规则；画线规则
{
    bool bRet = false;
    QJsonObject mainjson;
    mainjson.insert("org_no", "3");
    mainjson.insert("access_key", "");
    mainjson.insert("scope", "alarm");
    mainjson.insert("type", dictType);   //line_type :0,1,2   ;alarm_type:0,1,2

    QJsonDocument document;
    document.setObject(mainjson);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    HttpRequest httpreq;
    QString status_code="0";
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/alarm/getDict");
    //QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/static/getDict");
    QString rcvstr = httpreq.HttpSynPost(url,QString(byte_array),session_id,status_code);
    if(status_code!="200")
    {
        retCode=status_code;
        bRet=false;
        return bRet;
    }
    //"[{\"id\":56,\"scope\":\"alarm\",\"update_time\":null,\"remark\":null,\"name\":\"区域画线\",\"darea_id\":null,\"create_time\":null,\"code\":\"0\",\"type\":\"line_type\",\"dstatus\":1,\"oper_id\":null}]"
    //"[{\"id\":54,\"scope\":\"alarm\",\"update_time\":null,\"remark\":null,\"name\":\"区域逗留报警\",\"darea_id\":null,\"create_time\":null,\"code\":\"0\",\"type\":\"alarm_type\",\"dstatus\":1,\"oper_id\":null},{\"id\":55,\"scope\":\"alarm\",\"update_time\":null,\"remark\":null,\"name\":\"离开区域报警\",\"darea_id\":null,\"create_time\":null,\"code\":\"1\",\"type\":\"alarm_type\",\"dstatus\":1,\"oper_id\":null}]"
    if (!rcvstr.isEmpty())
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);

        if (json_error.error == QJsonParseError::NoError)
        {
            if (parse_doucment.isArray())
            {
                QJsonArray dataArray =  parse_doucment.array();
                for (int i = 0; i < dataArray.size(); i++)
                {
                    mDictionary diction;
                    QJsonValue dataArrayValue = dataArray.at(i);
                    if(dataArrayValue.isObject())
                    {
                        QJsonObject obj = dataArrayValue.toObject();
                        if(obj.contains("id")&&!obj.value("id").isNull())
                            diction.id = obj.value("id").toString().toInt();
                        if(obj.contains("scope")&&!obj.value("scope").isNull())
                            diction.scope = obj.value("scope").toString();
                        if(obj.contains("update_time")&&!obj.value("update_time").isNull())
                            diction.update_time = obj.value("update_time").toString();
                        if(obj.contains("remark")&&!obj.value("remark").isNull())
                            diction.remark = obj.value("remark").toString();
                        if(obj.contains("name")&&!obj.value("name").isNull())
                            diction.name = obj.value("name").toString();
                        if(obj.contains("darea_id")&&!obj.value("darea_id").isNull())
                            diction.darea_id = obj.value("darea_id").toString().toInt();
                        if(obj.contains("create_time")&&!obj.value("create_time").isNull())
                            diction.create_time = obj.value("create_time").toString();
                        if(obj.contains("code")&&!obj.value("code").isNull())
                            diction.code = obj.value("code").toString();
                        if(obj.contains("type")&&!obj.value("type").isNull())
                            diction.type = obj.value("type").toString();
                        if(obj.contains("dstatus")&&!obj.value("dstatus").isNull())
                            diction.dstatus = obj.value("dstatus").toInt();
                        if(obj.contains("oper_id")&&!obj.value("oper_id").isNull())
                            diction.oper_id = obj.value("oper_id").toString();
                        int key=diction.code.toInt() ;
                        if(diction.type.trimmed()=="line_type")
                            map_LineType.insert(key,diction);
                        else if(diction.type.trimmed()=="alarm_type")
                            map_AlarmType.insert(key,diction);
                    }
                }
                bRet = true;
            }
            else
            {
                bRet = false;
            }
        }
    }
    return bRet;
}

bool NetWorkSendManager::saveAlarmRule(mRule &data,QString &retCode)//保存报警规则
{
    bool bRet = false;
    QJsonObject mainjson;
    mainjson.insert("org_no", "3");
    mainjson.insert("access_key", "");
    mainjson.insert("rule_name", data.rule_name);
    if(data.rule_id=="0") mainjson.insert("rule_id", QString(""));
    else mainjson.insert("rule_id", QString("%1").arg(data.rule_id));
    mainjson.insert("camera_id", QString("%1").arg(data.camera_id));
    mainjson.insert("line_type", QString("%1").arg(data.line_type));
    mainjson.insert("alarm_type", QString("%1").arg(data.alarm_type));
    QString pointStr="";
    for(int i=0;i<data.points.size();i++)
    {
        if(i!=data.points.size()-1)
            pointStr+=QString("(%1,%2),").arg(data.points[i].x()).arg(data.points[i].y());
        else
            pointStr+=QString("(%1,%2)").arg(data.points[i].x()).arg(data.points[i].y());
    }
    mainjson.insert("points", pointStr);

    QString timesStr="";
    if(data.times.size()==1)
    {
        timesStr=QString("(%1)").arg(data.times[0]);
    }
    else if(data.times.size()==2)
    {
        timesStr=QString("(%1,%2)").arg(data.times[0]).arg(data.times[1]);
    }
    else if(data.times.size()>2)
    {
        for(int i=0;i<data.times.size();i++)
        {
            if(i==0) timesStr+=QString("(%1,").arg(data.times[i]);
            else if(i==data.times.size()-1) timesStr+=QString("%1)").arg(data.times[i]);
            else timesStr+=QString("%1,").arg(data.times[i]);
        }
    }
    mainjson.insert("times", timesStr);
    mainjson.insert("alarm_delay", QString("%1").arg(data.alarm_delay));
    mainjson.insert("alert_delay", QString("%1").arg(data.alert_delay));
    mainjson.insert("min_alarm_person_num", QString("%1").arg(data.alarm_person_num));
    mainjson.insert("max_alarm_person_num", QString("%1").arg(data.max_alarm_person_num));
    mainjson.insert("alarm_threshold", QString("%1").arg(data.alarm_threshold));
    mainjson.insert("alert_threshold", QString("%1").arg(data.alert_threshold));
    mainjson.insert("two_remind_time", QString("%1").arg(data.two_remind_time));

    QJsonDocument document;
    document.setObject(mainjson);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    HttpRequest httpreq;
    QString status_code="0";
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/alarm/saveAlarmCameraRule");
    QString rcvstr = httpreq.HttpSynPost(url,QString(byte_array),session_id,status_code);
    if(status_code!="200")
    {
        retCode=status_code;
        bRet=false;
        return bRet;
    }

    if (!rcvstr.isEmpty())
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);

        if (json_error.error == QJsonParseError::NoError)
        {
            QJsonValue valuetemp  = parse_doucment.object();
            if(valuetemp.isObject())
            {
                QJsonObject obj = valuetemp.toObject();

                if(obj.contains("rule_id")&&!obj.value("rule_id").isNull())
                {
                    data.rule_id=obj.value("rule_id").toString();
                }
            }

            else
            {
                bRet = false;
            }
        }
    }
    return bRet;

}

bool NetWorkSendManager::deleteAlarmRule(QString ruleID,QString &retCode)//删除报警规则
{
    bool bRet = false;
    QJsonObject mainjson;
    mainjson.insert("org_no", "3");
    mainjson.insert("access_key", "");
    mainjson.insert("rule_id", ruleID);
    QJsonDocument document;
    document.setObject(mainjson);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    HttpRequest httpreq;
    QString status_code="0";
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/alarm/removeAlarmCameraRule");
    QString rcvstr = httpreq.HttpSynPost(url,QString(byte_array),session_id,status_code);
    if(status_code!="200")
    {
        retCode=status_code;
        bRet=false;
        return bRet;
    }
    if (!rcvstr.isEmpty())
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);

        if (json_error.error == QJsonParseError::NoError)
        {
            QJsonValue valuetemp  = parse_doucment.object();
            QJsonObject obj = valuetemp.toObject();

            QJsonObject result_info_obj = obj.value("datas").toObject();
            if(result_info_obj.contains("datas"))
            {
                //arrays_value=result_info_obj.value("datas");
            }
            else
            {
                bRet = false;
            }
        }
    }
    return bRet;
}

bool NetWorkSendManager::saveAlarmRecord(mAlarmRecord_stu &data,QString &statusCode)//保存报警记录
{
    bool bRet = false;
    HttpRequest httpreq;
    QString status_code="0";
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/alarm/saveAlarmPre");
    QString rcvstr = httpreq.HttpSynMulitPartJPG(url,data,session_id,status_code);
    if(status_code!="200")
    {
        bRet=false;
    }
    else
    {
        bRet=true;
    }
    if (!rcvstr.isEmpty())
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);

        if (json_error.error == QJsonParseError::NoError)
        {
            QJsonValue valuetemp  = parse_doucment.object();
            QJsonObject obj = valuetemp.toObject();
            QString result_value;
            if(obj.contains("result"))
            {
                result_value=obj.value("result").toVariant().toString();
            }

            if(obj.contains("alarm_id"))
            {
                data.alarm_id=obj.value("alarm_id").toVariant().toString();
            }
        }
    }

    return bRet;
}

bool NetWorkSendManager::saveAlarmMP4(QString filename,QString &retCode,QString alarm_id,QString alarm_rule_id)//保存报警记录
{
    bool bRet = false;
    HttpRequest httpreq;
    QString status_code="0";
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/alarm/saveAlarmPre");
    QString rcvstr = httpreq.HttpSynMulitPart(url,filename,alarm_id,alarm_rule_id,session_id,status_code);
    if(status_code!="200")
    {
        retCode=status_code;
        bRet=false;
    }
    else
    {
        retCode=status_code;
        bRet=true;
    }
    if (!rcvstr.isEmpty())
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);

        if (json_error.error == QJsonParseError::NoError)
        {
            QJsonValue valuetemp  = parse_doucment.object();
            QJsonObject obj = valuetemp.toObject();
            QString result_value;
            if(obj.contains("result"))
            {
                result_value=obj.value("result").toVariant().toString();
            }
        }
    }
    return bRet;
}

bool NetWorkSendManager::HasAlarmDiscorrect(int &maxID, int &alarmDiscorrectCount)
{
    QJsonObject mainjson;
    mainjson.insert("org_no", "3");
    mainjson.insert("access_key", "");

    QJsonDocument document;
    document.setObject(mainjson);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    HttpRequest httpreq;
    QString status_code="0";
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/alarm/hasAlarmDiscorrect");
    QString rcvstr = httpreq.HttpSynPost(url,QString(byte_array),session_id,status_code);

    if(status_code!="200" || rcvstr.isEmpty())
    {
        LOG_ERROR("hasAlarmDiscorrect error %s.\n", status_code.toStdString().c_str());
        return false;
    }

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);

    if (json_error.error != QJsonParseError::NoError)// || !parse_doucment.isObject())
    {
        LOG_ERROR("parse_document error.\n");
        return false;
    }

    //QJsonValue arrays_value;
    QJsonValue value  = parse_doucment.object();
    if(!value.isObject())
    {
        LOG_ERROR("QJsonValue error.\n");
        return false;
    }

    QJsonObject obj = value.toObject();
    if(obj.contains("maxID")&&!obj.value("maxID").isNull())
        maxID = obj.value("maxID").toString().toInt();
    if(obj.contains("alarmDiscorrectCount")&&!obj.value("alarmDiscorrectCount").isNull())
        alarmDiscorrectCount = obj.value("alarmDiscorrectCount").toString().toInt();

    return true;
}

bool NetWorkSendManager::GetAlarmDiscorrect()
{    
    QJsonObject mainjson;
    mainjson.insert("org_no", "3");
    mainjson.insert("access_key", "");    

    QJsonDocument document;
    document.setObject(mainjson);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    HttpRequest httpreq;
    QString status_code="0";
    QString url=QString("%1%2").arg(g_Server_Adress).arg("/ivs-web/api/alarm/getAlarmDiscorrect");
    QString rcvstr = httpreq.HttpSynPost(url,QString(byte_array),session_id,status_code);

    if(status_code!="200" || rcvstr.isEmpty())
    {
        LOG_ERROR("getAlarmDiscorrect error %s.\n", status_code.toStdString().c_str());
        return false;
    }

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(rcvstr.toUtf8(), &json_error);

    if (json_error.error != QJsonParseError::NoError)// || parse_doucment.isObject())
    {
        LOG_ERROR("parse_document error.\n");
        return false;
    }

    QJsonValue arrays_value;
    QJsonValue valuetemp  = parse_doucment.object();
    if(!valuetemp.isObject())
    {
        LOG_ERROR("QJsonValue error.\n");
        return false;
    }

    QJsonObject result_info_obj = valuetemp.toObject();
    if(result_info_obj.contains("list"))
    {
        arrays_value=result_info_obj.value("list");

        if (arrays_value.isArray())//判断他是不是json数组
        {
            QJsonArray dataArray = arrays_value.toArray();
            for (int i = 0; i < dataArray.size(); i++)
            {
                QJsonValue dataArrayValue = dataArray.at(i);
                if(dataArrayValue.isObject())
                {
                    QJsonObject obj = dataArrayValue.toObject();

                    QString cameraID, ruleID, head_points;

                    if(obj.contains("camera_id")&&!obj.value("camera_id").isNull())
                        cameraID = obj.value("camera_id").toString();
                    if(obj.contains("rule_id")&&!obj.value("rule_id").isNull())
                        ruleID = obj.value("rule_id").toString();
                    if(obj.contains("head_points")&&!obj.value("head_points").isNull())
                        head_points = obj.value("head_points").toString();

                    QVector<QPoint> vecPoint;
                    QStringList strlist = head_points.split(',');
                    if(strlist.size()==4)
                    {
                        int x=strlist[0].toInt();
                        int y=strlist[1].toInt();
                        int w=strlist[2].toInt();
                        int h=strlist[3].toInt();

                        vecPoint.push_back(QPoint(x, y));
                        vecPoint.push_back(QPoint(x+w, y));
                        vecPoint.push_back(QPoint(x, y+h));
                        vecPoint.push_back(QPoint(x+w, y+h));
                    }

                    if(vecPoint.size()>0)
                        ALARM_MANAGER->SaveAlarmDiscorrect(cameraID+"_"+ruleID, vecPoint);
                }
            }
        }
    }


    return true;
}
