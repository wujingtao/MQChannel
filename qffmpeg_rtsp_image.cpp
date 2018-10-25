#include "qffmpeg_rtsp_image.h"
#include <QDir>
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
qffmpeg_rtsp_image::qffmpeg_rtsp_image()
{
   m_rtspAddress="";
   m_imageDirPath="";
   m_secondOfImageDealFrequency=0;

   m_ffm=NULL;
}

void qffmpeg_rtsp_image::setRtspAddress(QString address)      //rtsp路径
{
    m_rtspAddress=address;
}

QString qffmpeg_rtsp_image::getRtspAddress()
{
    return m_rtspAddress;
}

void qffmpeg_rtsp_image::setImageDirPath(QString imagePath)  //截图文件夹路径
{
    m_imageDirPath=imagePath;
}

QString qffmpeg_rtsp_image::getImageDirPath()
{
    return m_imageDirPath;
}

void qffmpeg_rtsp_image::setImageDealFrequency(int second)     //每秒保存的图片张数
{
    m_secondOfImageDealFrequency=second;
}

int  qffmpeg_rtsp_image::getImageDealFrequency()
{
    return m_secondOfImageDealFrequency;
}

bool qffmpeg_rtsp_image::executeGetImages(int i,int screenshotSelection)                      //开始截图
{
    if(m_rtspAddress.isEmpty())
    {
        qDebug()<<"m_rtspAddress can't be empty!";
    }

    if(m_imageDirPath.isEmpty())
    {
        qDebug()<<"m_imageDirPath can't be empty!";
    }

    if(0==m_secondOfImageDealFrequency)
    {
        qDebug()<<"m_imageDirPath can't be empty!";
    }
    QStringList paramstr;
    if(screenshotSelection == 1)
    {
        QString rtspAdress=m_rtspAddress;
        QStringList rtspList=rtspAdress.split("@");
        QStringList admin_pswd=rtspList[0].replace("rtsp://","").split(":");
        QStringList ip=rtspList[1].split(":554");
        QString profileToken=rtspAdress.split("_onviftoken_")[1];
        QString ipAdress=ip[0];
        QString admin=admin_pswd[0];
        QString passWD=admin_pswd[1];
        QString fileFodler=m_imageDirPath;
        qDebug()<<paramstr;
        paramstr<<"/home/mq/video/getsnapshot.py"<<ipAdress<<"80"<<admin<<passWD<<fileFodler<<profileToken<<QString("%1").arg(i);
        getImageProcess.execute(QString("/usr/bin/python"),paramstr);//command是要执行的命令,args是参数，
        if (!getImageProcess.waitForStarted())
        {
               qDebug()<<"error"<<QString::fromLocal8Bit(getImageProcess.readAllStandardError());
               return false;
        }
        ffmpegPID=getImageProcess.processId();
        //qDebug()<<"start ok,pid:"<<ffmpegPID;
        return true;
    }else
    {

//            paramstr<<"-i"<<m_rtspAddress.split("_onviftoken_")[0]<<"-r"<<QString("%1").arg(m_secondOfImageDealFrequency)
//               <<"-y"<<"-qscale:v"<<"1"<<"-s"<<"1280x720"<<"-f"<<"image2"<<QString("%1%05d.jpg").arg(m_imageDirPath);
//            getImageProcess.execute(QString("/usr/bin/ffmpeg"),paramstr);//command是要执行的命令,args是参数，
//            if (!getImageProcess.waitForStarted())
//            {
//                   qDebug()<<"error"<<QString::fromLocal8Bit(getImageProcess.readAllStandardError());
//                   return false;
//            }
//            ffmpegPID=getImageProcess.processId();

            return true;
    }
}

bool qffmpeg_rtsp_image::startGetImages(int i,int screenshotSelection)                      //开始截图
{
    if(m_rtspAddress.isEmpty())
    {
        qDebug()<<"m_rtspAddress can't be empty!";
    }

    if(m_imageDirPath.isEmpty())
    {
        qDebug()<<"m_imageDirPath can't be empty!";
    }

    if(0==m_secondOfImageDealFrequency)
    {
        qDebug()<<"m_imageDirPath can't be empty!";
    }
    QStringList paramstr;
    if(screenshotSelection == 1)
    {
        QString rtspAdress=m_rtspAddress;
        QStringList rtspList=rtspAdress.split("@");
        QStringList admin_pswd=rtspList[0].replace("rtsp://","").split(":");
        QStringList ip=rtspList[1].split(":554");
        QString profileToken=rtspAdress.split("_onviftoken_")[1];
        QString ipAdress=ip[0];
        QString admin=admin_pswd[0];
        QString passWD=admin_pswd[1];
        QString fileFodler=m_imageDirPath;

        paramstr<<"/home/mq/video/getsnapshot.py"<<ipAdress<<"80"<<admin<<passWD<<fileFodler<<profileToken<<QString("%1").arg(i);
        qDebug()<<paramstr;
        getImageProcess2.start(QString("/usr/bin/python"),paramstr);//command是要执行的命令,args是参数，
        if (!getImageProcess2.waitForStarted())
        {
               qDebug()<<"error"<<QString::fromLocal8Bit(getImageProcess2.readAllStandardError());
               return false;
        }
        ffmpegPID2=getImageProcess2.processId();
        return true;
    }else
    {
            paramstr<<"-i"<<m_rtspAddress.split("_onviftoken_")[0]<<"-r"<<QString("%1").arg(m_secondOfImageDealFrequency)
               <<"-y"<<"-qscale:v"<<"1"<<"-s"<<"1280x720"<<"-f"<<"image2"<<QString("%1%05d.jpg").arg(m_imageDirPath);
            qDebug()<<paramstr;
            getImageProcess2.start(QString("/usr/bin/ffmpeg"),paramstr);//command是要执行的命令,args是参数，
            if (!getImageProcess2.waitForStarted())
            {
                   qDebug()<<"error"<<QString::fromLocal8Bit(getImageProcess2.readAllStandardError());
                   return false;
            }
            ffmpegPID2=getImageProcess2.processId();
            return true;
    }
}

void qffmpeg_rtsp_image::stopGetImages()                       //停止截图
{
    getImageProcess2.terminate();
    QString cmd=QString("kill %1").arg(ffmpegPID2);
    system(cmd.toStdString().c_str());
}
