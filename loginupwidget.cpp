#include "loginupwidget.h"
#include "qmyglobal.h"
#include "datatranslate.h"
#include <QPainter>
#include <QPen>
#include <QRegExp>
#include <QValidator>
#include <QMessageBox>
#include <QDebug>
loginUpWindow::loginUpWindow(QWidget *parent) : QMainWindow(parent)
{  
    initWindow();
    initConfig();
}


void loginUpWindow::initWindow()
{
   // setMinimumSize(screenX,screenY/4*3);
    setMaximumSize(screenX,screenY/4*3);
    resize(screenX,screenY/4*3);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setStyleSheet("loginUpWindow{border-image:url(:/image/bg_1.png);}");

    //setStyleSheet("loginUpWindow{background:rgb(151,71,115);border:2px groove gray;border-radius:10px;padding:2px 4px;}");



    labLogoFont=new QLabel(this);
    labLogoFont->setFixedSize(screenX*0.49,screenY/4*3*0.76);
    labLogoFont->setPixmap(QPixmap(":/image/big_juxing.png"));

    labLogo=new QLabel(this);
    labLogo->setFixedSize(screenX*0.2,screenY/4*3*0.76);
    labLogo->setPixmap(QPixmap(":/image/camera.png"));

    companyLogo=new QLabel(this);
    companyLogo->setFixedSize(screenX*0.2,screenY/4*3*0.2);
    companyLogo->setPixmap(QPixmap(":/image/logo.png"));

    companyTips=new QLabel(this);
    companyTips->setFixedSize(screenX*0.3,screenY/4*3*0.3);
    companyTips->setPixmap(QPixmap(":/image/T.png"));

    companyLine=new QLabel(this);
    companyLine->setFixedSize(screenX*0.1,screenY/4*3*0.33);
    companyLine->setPixmap(QPixmap(":/image/line.png"));


    QFont ft;
    ft.setPointSize(15);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::darkGray);

    labUserName=new QLabel(this);
    labUserName->setFixedSize(100,50);
    labUserName->setFont(ft);
    labUserName->setPalette(pa);
    labUserName->setText("用户名:");


    labPasswd=new QLabel(this);
    labPasswd->setFixedSize(100,50);
    labPasswd->setFont(ft);
    labPasswd->setPalette(pa);
    labPasswd->setText("密    码:");

    lineEdUserName=new QLineEdit(this);
    lineEdUserName->setFixedSize(250,33);
    lineEdUserName->setText(userName);
    lineEdUserName->setPlaceholderText("请输入用户名");

    lineEdPasswd=new QLineEdit(this);
    lineEdPasswd->setFixedSize(250,34);
    lineEdPasswd->setText(passWD);
    lineEdPasswd->setEchoMode(QLineEdit::Password);
    lineEdPasswd->setPlaceholderText("请输入密码");

    buttonLogin=new QPushButton(this);
    buttonLogin->setFixedSize(110,35);
    buttonLogin->setText("登录");
   // buttonLogin->setStyleSheet(tr("border-image: url(:/image/button.png);"));

    buttonExit=new QPushButton(this);
    buttonExit->setFixedSize(110,35);
    buttonExit->setText("退出");
   // buttonExit->setStyleSheet(tr("border-image: url(:/image/button.png);"));

    buttonConfig=new QPushButton(this);
    buttonConfig->setFixedSize(110,35);
    buttonConfig->setText("打开服务器配置");
   // buttonConfig->setStyleSheet(tr("border-image: url(:/image/button.png);"));

    labMessage=new QLabel(this);
    pa.setColor(QPalette::WindowText,Qt::red);
    labMessage->setPalette(pa);
    labMessage->setFixedSize(200,32);
    labMessage->setText("");

    labsmallBox=new QLabel(this);
    labsmallBox->setFixedSize(screenX*0.25,screenY/4*3*0.3);
    labsmallBox->setPixmap(QPixmap(":/image/small-juxing.png"));

    softwareName=new QLabel(this);
    softwareName->setFixedSize(screenX*0.25,screenY/4*3*0.1);
    pa.setColor(QPalette::WindowText, QColor(32,140,202));
    ft.setPointSize(20);
    softwareName->setFont(ft);
    softwareName->setPalette(pa);
    softwareName->setText("湖南敏求AI智能视频分析系统");


    closeButton=new main3Button(":/image/close.png",this);
    minButton=new main3Button(":/image/min.png",this);
    closeButton->setFixedSize(40,36);
    minButton->setFixedSize(40,36);

    location = this->geometry();
    labLogoFont->setGeometry(location.width()*0.25,location.height()*0.22,labLogoFont->width(),labLogoFont->height());
    labLogo->setGeometry(location.width()*0.28,location.height()*0.22,labLogo->width(),labLogo->height());
    companyLogo->setGeometry(location.width()*0.33,location.height()*0.36,companyLogo->width(),companyLogo->height());
    companyTips->setGeometry(location.width()*0.31,location.height()*0.52,companyTips->width(),companyTips->height());
    companyLine->setGeometry(location.width()*0.47,location.height()*0.45,companyLine->width(),companyLine->height());
    labMessage->setGeometry(location.width()*0.5,location.height()*0.8,labMessage->width(),labMessage->height());
    labUserName->setGeometry(location.width()*0.48,location.height()*0.45,labUserName->width(),labUserName->height());
    labPasswd->setGeometry(location.width()*0.48,location.height()*0.52,labPasswd->width(),labPasswd->height());
    lineEdUserName->setGeometry(location.width()*0.52,location.height()*0.46,lineEdUserName->width(),lineEdUserName->height());
    lineEdPasswd->setGeometry(location.width()*0.52,location.height()*0.53,lineEdPasswd->width(),lineEdPasswd->height());
    buttonLogin->setGeometry(location.width()*0.52,location.height()*0.6,buttonLogin->width(),buttonLogin->height());
    buttonExit->setGeometry(location.width()*0.54+buttonLogin->width(),location.height()*0.6,buttonExit->width(),buttonExit->height());
    buttonConfig->setGeometry(location.width()*0.52,location.height()*0.67,buttonConfig->width(),buttonConfig->height());
    labsmallBox->setGeometry(location.width()*0.52+buttonConfig->width(),location.height()*0.63,labsmallBox->width(),labsmallBox->height());
    softwareName->setGeometry(location.width()*0.5,location.height()*0.33,softwareName->width(),softwareName->height());
    closeButton->setGeometry(location.width()*0.95,location.height()*0.02,closeButton->width(),closeButton->height());
    minButton->setGeometry(location.width()*0.90,location.height()*0.02,closeButton->width(),closeButton->height());
    connect(buttonConfig,SIGNAL(clicked()),this,SLOT(slot_ConfigButton()));
}

void loginUpWindow::initConfig()
{
    labseverIp=new QLabel(this);
    labseverIp->setFixedSize(110,30);
    labseverIp->setText("服务器IP地址：");

    lineEdIP = new CIPLineEdit(this);
    lineEdIP->setGeometry(50, 50, 120, 26);

    labseverPort=new QLabel(this);
    labseverPort->setFixedSize(110,30);
    labseverPort->setText("服务器端口：");

    lineEdPort=new QLineEdit(this);
    lineEdPort->setFixedSize(40,30);
    lineEdPort->setText(g_Port);
    QRegExp regExp = QRegExp("[0-9][0-9][0-9][0-9][0-9]");
    lineEdPort->setValidator(new QRegExpValidator(regExp,lineEdPort));

    labseverName=new QLabel(this);
    labseverName->setFixedSize(110,30);
    labseverName->setText("分析服务器号：");

    QRegExp regExpSever = QRegExp("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
    lineEdseverName=new QLineEdit(this);
    lineEdseverName->setFixedSize(40,30);
    lineEdseverName->setText(g_AnalyseServerID);
    lineEdseverName->setValidator(new QRegExpValidator(regExpSever,lineEdPort));

    saveConfigButton=new QPushButton(this);
    saveConfigButton->setFixedSize(90,35);
    saveConfigButton->setText("保存配置");

    QStringList list=g_IP.split(".");
    if(list.size()==4)
    {
    lineEdIP->SetIPText(list[0].toInt(),0);
    lineEdIP->SetIPText(list[1].toInt(),1);
    lineEdIP->SetIPText(list[2].toInt(),2);
    lineEdIP->SetIPText(list[3].toInt(),3);
    }

    g_Server_Adress=QString("http://%1:%2").arg(g_IP).arg(g_Port);

    labseverIp->setGeometry(location.width()*0.53+buttonConfig->width(),location.height()*0.67,labseverIp->width(),labseverIp->height());
    lineEdIP->setGeometry(location.width()*0.53+buttonConfig->width()+labseverIp->width(),location.height()*0.67,lineEdIP->width(),lineEdIP->height());
    labseverPort->setGeometry(location.width()*0.53+buttonConfig->width(),location.height()*0.72,labseverPort->width(),labseverPort->height());
    lineEdPort->setGeometry(location.width()*0.53+buttonConfig->width()+labseverIp->width(),location.height()*0.72,lineEdPort->width(),lineEdPort->height());
    labseverName->setGeometry(location.width()*0.53+buttonConfig->width(),location.height()*0.77,labseverName->width(),labseverName->height());
    lineEdseverName->setGeometry(location.width()*0.53+buttonConfig->width()+labseverName->width(),location.height()*0.77,lineEdseverName->width(),lineEdseverName->height());
    saveConfigButton->setGeometry(location.width()*0.6+buttonConfig->width(),location.height()*0.82,saveConfigButton->width(),saveConfigButton->height());


    connect(saveConfigButton,SIGNAL(clicked()),this,SLOT(slot_SaveConfig()));
    labseverIp->hide();
    lineEdIP->hide();
    labseverPort->hide();
    lineEdPort->hide();
    labseverName->hide();
    lineEdseverName->hide();
    saveConfigButton->hide();
    labsmallBox->hide();
}

bool loginUpWindow::isLogin()
{
    if(lineEdUserName->text().isEmpty())
    {
        labMessage->setText("请输入用户名");
        return false;
    }

    if(lineEdPasswd->text().isEmpty())
    {
        labMessage->setText("请输入密码");
        return false;
    }

    QStringList list=g_IP.split(".");
    if(list.size()!=4||g_Port.isEmpty())
    {
        labMessage->setText("请重新配置新的服务器地址");
        ConfigButton2();
        return false;
    }

    userName=lineEdUserName->text();
    passWD=lineEdPasswd->text();
    QString retCode="";
    buttonLogin->setText("正在登录");
    buttonLogin->setEnabled(false);
    bool ret=DATA_TRANSLATE->login(retCode);
    if(ret)
    {
        buttonLogin->setText("登录");
        buttonLogin->setEnabled(true);
        WriteSettings(userName,passWD,g_IP,g_Port,g_AnalyseServerID);
        return true;
    }
    else
    {
        buttonLogin->setText("登录");
        buttonLogin->setEnabled(true);
         if(retCode.isEmpty())
             labMessage->setText("服务器连接异常");
         else
             labMessage->setText("用户名或密码错误");
         for(int i=20;i>0;i--)
         {
             if(exitapp)
             {
                  qDebug()<<QDateTime::currentDateTime()<<"exit";
                  return 0;
             }
            QString str=QString("%1秒后重新登录").arg(i);
            labMessage->setText(str);
            delaymsec(1000);
         }
         ret =isLogin();
         return ret;
    }
}

void loginUpWindow::ConfigButton2()
{
    if(buttonConfig->text()=="收起服务器配置")
    {
        QStringList list=g_IP.split(".");
        if(list.size()==4)
        {
        lineEdIP->SetIPText(list[0].toInt(),0);
        lineEdIP->SetIPText(list[1].toInt(),1);
        lineEdIP->SetIPText(list[2].toInt(),2);
        lineEdIP->SetIPText(list[3].toInt(),3);
        }
        lineEdPort->setText(g_Port);
        lineEdseverName->setText(g_AnalyseServerID);
        buttonConfig->setText("打开服务器配置");
        labseverIp->hide();
        lineEdIP->hide();
        labseverPort->hide();
        lineEdPort->hide();
        labseverName->hide();
        lineEdseverName->hide();
        saveConfigButton->hide();
        labsmallBox->hide();
    }
}

void loginUpWindow::slot_ConfigButton()
{
    if(buttonConfig->text()=="收起服务器配置")
    {
        QStringList list=g_IP.split(".");
        if(list.size()==4)
        {
        lineEdIP->SetIPText(list[0].toInt(),0);
        lineEdIP->SetIPText(list[1].toInt(),1);
        lineEdIP->SetIPText(list[2].toInt(),2);
        lineEdIP->SetIPText(list[3].toInt(),3);
        }
        lineEdPort->setText(g_Port);
        lineEdseverName->setText(g_AnalyseServerID);
        buttonConfig->setText("打开服务器配置");
        labseverIp->hide();
        lineEdIP->hide();
        labseverPort->hide();
        lineEdPort->hide();
        labseverName->hide();
        lineEdseverName->hide();
        saveConfigButton->hide();
        labsmallBox->hide();
    }
    else if(buttonConfig->text()=="打开服务器配置")
    {
        QStringList list=g_IP.split(".");
        if(list.size()==4)
        {
        lineEdIP->SetIPText(list[0].toInt(),0);
        lineEdIP->SetIPText(list[1].toInt(),1);
        lineEdIP->SetIPText(list[2].toInt(),2);
        lineEdIP->SetIPText(list[3].toInt(),3);
        }
        lineEdPort->setText(g_Port);
        lineEdseverName->setText(g_AnalyseServerID);
        buttonConfig->setText("收起服务器配置");
        labseverIp->setVisible(true);
        lineEdIP->setVisible(true);
        labseverPort->setVisible(true);
        lineEdPort->setVisible(true);
        labseverName->setVisible(true);
        lineEdseverName->setVisible(true);
        saveConfigButton->setVisible(true);
        labsmallBox->setVisible(true);
    }
}

void loginUpWindow::slot_SaveConfig()
{
     QString IPadress="";
     if(lineEdIP->GetIPText(0)>=0&&lineEdIP->GetIPText(0)<=255&&
        lineEdIP->GetIPText(1)>=0&&lineEdIP->GetIPText(1)<=255&&
        lineEdIP->GetIPText(2)>=0&&lineEdIP->GetIPText(2)<=255&&
        lineEdIP->GetIPText(3)>=0&&lineEdIP->GetIPText(3)<=255)
     {
     IPadress=QString("%1.%2.%3.%4").arg(lineEdIP->GetIPText(0)).arg(lineEdIP->GetIPText(1)).arg(lineEdIP->GetIPText(2)).arg(lineEdIP->GetIPText(3));
     }
     else
     {
         QMessageBox::about(NULL, "ip校验错误", "请输入正确的IP地址");
         return ;
     }

     QString Port=lineEdPort->text();
     QString analyseServerID=lineEdseverName->text();
     if(Port.isEmpty())
     {
         QMessageBox::about(NULL, "端口校验错误", "请输入正确的端口");
         return ;
     }
     
     g_IP=IPadress;
     g_Port=Port;
     g_AnalyseServerID=analyseServerID;
     WriteSettings(userName,passWD,g_IP,g_Port,g_AnalyseServerID);
     g_Server_Adress=QString("http://%1:%2").arg(g_IP).arg(g_Port);
}



COneIPLineEdit::COneIPLineEdit( QWidget *parent )
:QLineEdit(parent)
{
    setAlignment(Qt::AlignHCenter);
    setMaxLength(3);
    setMinimumSize(25, 20);
    QRegExp rx("(2[0-5]{2}|2[0-4][0-9]|1?[0-9]{1,2})");
    setValidator(new QRegExpValidator(rx, this));
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(SlotTextChanged()));
}
COneIPLineEdit::~COneIPLineEdit()
{
}
//void COneIPLineEdit::SlotPreFinished()
//{
//
//}
//
//void COneIPLineEdit::SlotTextChanged()
//{
//  if (text().length() == 3)
//  {
//
//  }
//}
/*******************************************************************************************/
CIPLineEdit::CIPLineEdit(QWidget *parent)
: QLineEdit(parent)
{

}
CIPLineEdit::~CIPLineEdit()
{
}
void CIPLineEdit::paintEvent( QPaintEvent *e )
{
    QLineEdit::paintEvent(e);

    int nWidth = width() - 5;//3个点，两头边框各一个像素
    int nAverageWidth = nWidth / 4;
    int nPointY = height() / 2;
    int nTrans = 0;
    //若除以4且算上点、框宽度之后仍有2或3的空余，则左边框留两个像素显示
    if (nWidth - nAverageWidth * 4 - 5 >= 2)
    {
        nTrans = 1;
    }
    QPainter painter(this);
    painter.setPen(QPen(Qt::black));
    painter.save();
    for (int i = 0; i < 3; i++)
    {
        painter.drawPoint(nAverageWidth*(i+1) + i+1 + nTrans, nPointY);
    }
    painter.restore();
}
void CIPLineEdit::setGeometry( int x, int y, int w, int h )
{
    QLineEdit::setGeometry(x, y, w, h);
    int nWidth = w - 5;//3个点，两头边框各一个像素
    int nAverageWidth = nWidth / 4;
    int nAverageHeight = h - 2;
    int nTrans = 0;
    //若除以4且算上点、框宽度之后仍有2或3的空余，则左边框留两个像素显示
    if (nWidth - nAverageWidth * 4 - 5 >= 2)
    {
        nTrans = 1;
    }
    for (int i = 0; i < 4; i++)
    {
        m_pLineEdit[i] = new COneIPLineEdit(this);
        m_pLineEdit[i]->setFrame(false);
        m_pLineEdit[i]->setGeometry(nAverageWidth*i+ i + 1, 1, nAverageWidth, nAverageHeight);
    }
}
bool CIPLineEdit::SetIPText( int nIP, int nIndex )
{
    if (nIndex < 0 || nIndex > 3)
    {
        return false;
    }
    m_pLineEdit[nIndex]->setText(QString("%1").arg(nIP));
    return true;
}
int CIPLineEdit::GetIPText( int nIndex )
{
    if (nIndex < 0 || nIndex > 3||m_pLineEdit[nIndex]->text().isEmpty())
    {
        return -1;
    }
    return m_pLineEdit[nIndex]->text().toInt();
}
