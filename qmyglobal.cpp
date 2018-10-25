#include "qmyglobal.h"
#include <QPainter>
#include <QElapsedTimer>
#include <QApplication>
#include <QSettings>

int screenX;
int screenY;
QString userName;
QString passWD;
QString session_id;
//QLogFile *g_pLogFile;
QMap<int,mDictionary> map_AlarmType; //报警类型队列
QMap<int,mDictionary> map_LineType; //画线类型队列
//QMap<QString,mMachine> map_Machine;
QMap<QString,mArea> map_Area; //存储从上端获取的Area队列
RuleMap g_rules;
QMap<QString,mRule> map_CutImageRule;//单独存储摄像头的有效范围队列
QMutex caffe_mutex;
QMutex caffe_mutex2;
QMutex caffe_mutex3;
QMutex mapAfterDeal_mutex;
QMutex mapAfterDeal_mutex2;
QMutex mapAfterDeal_mutex3;
QMutex caffeDeal_mutex;
bool exitapp;
QString g_IP;
QString g_Port;
QString g_Server_Adress;
QString g_AnalyseServerID;
double window_width_video;
double window_height_video;
bool islogin;

QMutex upMutex;
QMutex afterupMutex;
QList<stru_uploadData> list_upload;
QList<stru_uploadData> list_afterUpload;


void delaymsec(int ms)
{
    QElapsedTimer timer;
    timer.start();

    while (!timer.hasExpired(ms))
        QApplication::processEvents();

}



qmyglobal::qmyglobal()
{

}


main3Button::main3Button(QString pixnormal,QWidget *parent):QPushButton(parent)
{
    this->setCursor(Qt::PointingHandCursor);
    m_index=0;
     m_enter=false;
    QPixmap pix(pixnormal);
    for(int i=0;i<4;i++)
    m_pixlist<<pix.copy(i*(pix.width()/4),0,pix.width()/4,pix.height());
}
void main3Button::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap((width()-m_pixlist.at(m_index).width())/2,(height()-m_pixlist.at(m_index).height())/2
                             ,m_pixlist.at(m_index).width()
                             ,m_pixlist.at(m_index).height(),m_pixlist.at(m_index));//画图画到中间
}
void main3Button::enterEvent(QEvent *)
{
    m_index=1;
    m_enter=true;
    update();
}
void main3Button::leaveEvent(QEvent *)
{
    m_index=0;
    m_enter=false;
    update();
}
void main3Button::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)//如果是左键按下
    {
        m_index=2;
        update();
        QPushButton::mousePressEvent(e);//返回到上级
    }

}
void main3Button::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)//如果是左键放下
    {
        m_index=1;
        update();
       QPushButton::mouseReleaseEvent(e);//返回到上级 引出按钮clicked信号
    }

}

void WriteSettings(QString userName,QString passwd,QString serverIP ,QString serverPort,QString serverID)
{
    QString dir=QApplication::applicationDirPath();
    QFile file(dir+"/Config.ini");
    if (!file.open(QIODevice::WriteOnly))
    {
    }
    file.close();
    QSettings settings(dir+"/Config.ini", QSettings::IniFormat); // 当前目录的INI文件
    settings.beginGroup("UserConfig");
    settings.setValue("SERVER_USERNAME", QVariant(userName));
    settings.setValue("SERVER_PASSWD", QVariant(passwd));
    settings.setValue("SERVER_IP", QVariant(serverIP));
    settings.setValue("SERVER_PORT", QVariant(serverPort));
    settings.setValue("SERVER_ID", QVariant(serverID));
    settings.endGroup();
}

void ReadSettings(QString &username,QString &passwd,QString &serverIP ,QString &serverPort,QString &serverID)
{
    QString dir=QApplication::applicationDirPath();
    QFile file(dir+"/Config.ini");
    if(!file.exists())
    {
        if (!file.open(QIODevice::WriteOnly))
        {
        }
        file.close();
    }
    QSettings settings(dir+"/Config.ini", QSettings::IniFormat);
    username=settings.value("UserConfig/SERVER_USERNAME").toString();
    passwd=settings.value("UserConfig/SERVER_PASSWD").toString();
    serverIP = settings.value("UserConfig/SERVER_IP").toString();
    serverPort = settings.value("UserConfig/SERVER_PORT").toString();
    serverID = settings.value("UserConfig/SERVER_ID").toString();
}

cv::Mat QImageToMat(QImage image)
{
    cv::Mat mat;
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}


QImage MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB32);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}
