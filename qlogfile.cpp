#include "qlogfile.h"

QLogFile::QLogFile()
{

}

QLogFile::~QLogFile()
{

}

void QLogFile::InitLogFile(QString strPathName)
{
    m_strPathName = strPathName;
}

//储存日志函数，主要是存文件和更新界面上的日志内容
//MessageLevel 参数 必须是 Message_Level 类型
void QLogFile::SaveLogFile(QString strLog,int MessageLevel)
{
    m_mutex.lock();

    QFile file(m_strPathName);
    bool bIsExist = file.exists();
    if(bIsExist)
    {
        file.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    else
    {
        QDir toMakePath;
        toMakePath.mkpath("./log");
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    }


    QDateTime tm = QDateTime::currentDateTime();
    QString strTime = tm.toString("yyyy-MM-dd hh:mm:ss");

    if(file.size()>500000)
    {
        file.resize(0);
    }

    QString str;
    str += strTime;
    str += " ";
    str += strLog;
    str += "\r\n";
    file.write(str.toLocal8Bit().data(), str.toLocal8Bit().length());

    file.close();

    //发到主界面的bottomListView中
    emit logToWidgetEvent(strLog,MessageLevel);

    m_mutex.unlock();
}
