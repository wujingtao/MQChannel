#include "mainwindow.h"
#include <QApplication>
#include <../singleapplication/singleapplication.h>
#include <QSharedMemory>
#include <QMessageBox>
#include <QDebug>
#include "qmyglobal.h"
#include <QDesktopWidget>
#include <QSqlDatabase>
#include <QBuffer>
#include "qffmpeg_rtsp_image.h"
#include <QTextCodec>
#include <QImageReader>
#include "objectdetector.h"
#include "imgdisplay.h"
#include "ffmpegdecode.h"

#include "imageserver.h"
#include "param.h"
#include "caffeserver.h"

#include "Logger.h"

#include "cameraserver.h"
#include "imageanalyse.h"
#include "uploadserver.h"
#include "GlobalData.h"
#include "AlarmServer.h"

int main(int argc, char *argv[])
{
    SingleApplication a( argc, argv);

    Logger::init("log_analyse", LL_DEBUG);


    //1.初始参数
    screenX=qApp->desktop()->availableGeometry().width();
    screenY=qApp->desktop()->availableGeometry().height();
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    delaymsec(9000);
    //初始化日志位置 TODO:可删
    //全局日志使用方式，便于调试 在需要打出日志的地方调用  g_pLogFile->SaveLogFile();
    //QString logFullPath = "./log/video.log";
    //g_pLogFile = new QLogFile();
    //g_pLogFile->InitLogFile(logFullPath);
    ReadSettings(userName,passWD,g_IP,g_Port,g_AnalyseServerID);

    //2.登录
    MainWindow w;
    w.show();
    while(!islogin)   //不能登录则退出
    {
       if(exitapp)
       {
            qDebug()<<QDateTime::currentDateTime()<<"exit";
            return 0;
       }
       delaymsec(1000);
    }
    delaymsec(3000);

    //3.图像处理
    QString path=QString("/home/mq/video");
    QDir dir(path);
    if(!dir.exists()) dir.mkdir(path);

    UploadServer upserver;
    upserver.start();

    //new add
    AlarmServer *alarmServer = new AlarmServer;
    alarmServer->start();
    //5.图像传输服务
    ImageServer *imageserver = new ImageServer;
    imageserver->start();

    //4.实时图像
    ChannelVector vecChannel[OBJECT_CAFFE_COUNT];

    ffmpegDecode::Register();

    int channelNum=0;
    IPCMap mapChannel = GLOBAL_DATA->GetChannel();
    for(IPCMap::iterator iter = mapChannel.begin(); iter!=mapChannel.end() && channelNum<CHANNEL_COUNT; ++iter, ++channelNum)
    {        
        vector<mIPC> &vecIPC = iter->second;
        for(size_t i=0; i<vecIPC.size(); i++)
        {
            QString ipcid = vecIPC[i].IPCId;
            QString rtsp = vecIPC[i].IPCAdress.split("_onviftoken_")[0];

            QString path=QString("/home/mq/video/%1").arg(ipcid);
            QDir dir(path);
            if(!dir.exists()) dir.mkdir(path);

            ffmpegDecode *pFfm = new ffmpegDecode(PIC_WIDTH, PIC_HEIGHT);
            imageAnalyse * imageAna=new imageAnalyse(ipcid, path);

            tagCameraArg tagCamera;
            tagCamera.pFfm=pFfm;
            tagCamera.pAnalyse=imageAna;
            tagCamera.ipcid=ipcid;
            tagCamera.rtsp=rtsp;

            GLOBAL_DATA->SaveCameraArg(iter->first, tagCamera);

            LOG_INFO("channelID:%d, ipcid: %s, minutes:%d, st:%s, et:%s.\n", iter->first, ipcid.toStdString().c_str(),
                     vecIPC[i].minutes, vecIPC[i].start_time.toString("hh:mm:ss").toStdString().c_str(),
                     vecIPC[i].end_time.toString("hh:mm:ss").toStdString().c_str());
        }

        vecChannel[channelNum%OBJECT_CAFFE_COUNT].push_back(iter->first);


        //1.存储原始图片
        CameraServer * cameraServer=new CameraServer(iter->first, iter->second);
        cameraServer->start();

        LOG_INFO("正在启用channel:%d分析,camera count:%d!\n", iter->first, vecIPC.size());
    }

    //
    for(int i=0; i<OBJECT_CAFFE_COUNT && i<CHANNEL_COUNT; i++)
    {           
        CaffeServer *pCaffeServer = new CaffeServer(vecChannel[i]);
        pCaffeServer->start();                
    }

    w.setGeometry(qApp->desktop()->availableGeometry());

    return a.exec();
}
