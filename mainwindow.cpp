#include "mainwindow.h"
#include "qmyglobal.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include "datatranslate.h"
MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent)
{
    initWindow();
    initLoginWindow();
    initSystemTray();
    islogin=false;
    slot_hideanimation();
    //autoLogin();
}



//初始化窗口
void MainWindow::initWindow()
{
   // setMinimumSize(screenX,screenY);
    setMaximumSize(screenX,screenY);
    resize(screenX,screenY);
    setWindowFlags(Qt::FramelessWindowHint); //不带标题栏
    setWindowIcon(QIcon(":/image/LOGO1.png"));
    this->setObjectName("mainWindow");
    this->setStyleSheet("#mainWindow{border-image:url(:/image/background.png);}");
    //this->setStyleSheet("#mainWindow{background:rgb(217,95,124);border:2px groove gray;border-radius:10px;padding:2px 4px;}");
    m_MousePressed=false;

    companyLogo=new QLabel(this);
    companyLogo->setFixedSize(screenX*0.06,screenY*0.095);
    QPixmap imgTmp = QPixmap(":/image/LOGO1.png").scaled(110,100);
    companyLogo->setPixmap(imgTmp);

    location = this->geometry();

    m_stackwid=new QStackedWidget(this);   //堆栈窗口,多页面切换，暂时只需要配置界面，预留便于拓展新功能
    m_stackwid->setWindowFlags(Qt::FramelessWindowHint);
    m_configwid=new ConfigWidget(m_stackwid);

    m_videowid=new VideoWidget(m_stackwid);

    m_logwid=new logwidget(m_stackwid);
    m_logwid->setFixedSize(screenX,screenY*0.91);

    m_stackwid->addWidget(m_logwid);
    m_stackwid->addWidget(m_configwid);
    m_stackwid->addWidget(m_videowid);

    m_stackwid->setGeometry(0,screenY*0.086,screenX,screenY*0.91);
    m_stackwid->setCurrentIndex(0);       //配置界面

    videoButton=new QPushButton(this);
    videoButton->setFixedSize(screenX*0.08,screenY*0.04);
    videoButton->setObjectName("videobutton");
    videoButton->setStyleSheet("#videobutton{border-image: url(:/image/button4.png);}"
                               "#videobutton:hover{border-image: url(:/image/button.png);}"
                               "#videobutton:pressed{border-image: url(:/image/button5.png);}");
    videoButton->setToolTip("视频监控");
    videoButton->setText("视频监控");

    configButton=new QPushButton(this);
    configButton->setFixedSize(screenX*0.08,screenY*0.04);
    configButton->setObjectName("configbutton");
    configButton->setStyleSheet("#configbutton{border-image: url(:/image/button4.png);}"
                                "#configbutton:hover{border-image: url(:/image/button.png);}"
                                "#configbutton:pressed{border-image: url(:/image/button5.png);}");
    configButton->setToolTip("系统配置");
    configButton->setText("系统配置");

    logButton=new QPushButton(this);
    logButton->setFixedSize(screenX*0.08,screenY*0.04);
    logButton->setObjectName("configbutton");
    logButton->setStyleSheet("#configbutton{border-image: url(:/image/button5.png);}"
                                "#configbutton:hover{border-image: url(:/image/button.png);}"
                                "#configbutton:pressed{border-image: url(:/image/button4.png);}");
    logButton->setToolTip("运行日志");
    logButton->setText("运行日志");


    connect(videoButton,SIGNAL(clicked()),this,SLOT(changed_videowid()));
    connect(configButton,SIGNAL(clicked()),this,SLOT(changed_configwid()));
    connect(logButton,SIGNAL(clicked()),this,SLOT(changed_logwid()));

    closeButton=new main3Button(":/image/close.png",this);
    minButton=new main3Button(":/image/min.png",this);
    closeButton->setFixedSize(40,36);
    minButton->setFixedSize(40,36);

    companyLogo->setGeometry(location.width()*0.05,0,companyLogo->width(),companyLogo->height());
    logButton->setGeometry(location.width()*0.18,screenY*0.02,logButton->width(),logButton->height());
    configButton->setGeometry(location.width()*0.23+logButton->width(),screenY*0.02,configButton->width(),configButton->height());
    videoButton->setGeometry(location.width()*0.28+logButton->width()+configButton->width(),screenY*0.02,videoButton->width(),videoButton->height());
    closeButton->setGeometry(location.width()*0.95,location.height()*0.02,closeButton->width(),closeButton->height());
    minButton->setGeometry(location.width()*0.90,location.height()*0.02,closeButton->width(),closeButton->height());

   timer_show=new QTimer(this);
   connect(timer_show,SIGNAL(timeout()),m_videowid,SLOT(readImg()));
   timer_show->start(900);
    //不显示视频窗口，只截图做配置
   // videoButton->setVisible(false);
}

//初始化登录窗口
void MainWindow::initLoginWindow()
{
    maskmainwid=new loginmaskwindow(this);
    maskmainwid->setGeometry(0,0,screenX,screenY);

    ////////////////////////////显示登录面动画
    m_loginupwincloseani=new QPropertyAnimation(maskmainwid->loginupwd,"pos");
    m_logindownwincloseani=new QPropertyAnimation(maskmainwid->logindownwd,"pos");

    m_loginupwincloseani->setStartValue(QPoint(0,0));
    m_loginupwincloseani->setEndValue(QPoint(0,-screenY/4*3));
    m_loginupwincloseani->setDuration(200);

    m_logindownwincloseani->setStartValue(QPoint(0,screenY/4*3));
    m_logindownwincloseani->setEndValue(QPoint(0,screenX));
    m_logindownwincloseani->setDuration(200);


    ///////////////////////////用于关闭动画
    m_loginupwinshowani=new QPropertyAnimation(maskmainwid->loginupwd,"pos");
    m_logindownwinshowani=new QPropertyAnimation(maskmainwid->logindownwd,"pos");

    m_loginupwinshowani->setStartValue(QPoint(0,-screenY/4*3));
    m_loginupwinshowani->setEndValue(QPoint(0,0));
    m_loginupwinshowani->setDuration(400);

    m_logindownwinshowani->setStartValue(QPoint(0,screenX));
    m_logindownwinshowani->setEndValue(QPoint(0,screenY/4*3));
    m_logindownwinshowani->setDuration(400);

    connect(m_loginupwincloseani,SIGNAL(finished()),this,SLOT(slot_hideanimationfinished()));
    connect(m_logindownwincloseani,SIGNAL(finished()),this,SLOT(slot_hideanimationfinished()));
    connect(m_loginupwinshowani,SIGNAL(finished()),this,SLOT(slot_showanimationfinished()));
    connect(m_logindownwinshowani,SIGNAL(finished()),this,SLOT(slot_showanimationfinished()));

    connect(maskmainwid->loginupwd->buttonLogin,SIGNAL(clicked()),this,SLOT(slot_hideanimation()));
    connect(maskmainwid->loginupwd->buttonExit,SIGNAL(clicked()),this,SLOT(closeApp()));
    connect(maskmainwid->loginupwd->minButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
    connect(maskmainwid->loginupwd->closeButton,SIGNAL(clicked()),this,SLOT(closeApp()));
    connect(minButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
    connect(closeButton,SIGNAL(clicked()),this,SLOT(closeApp()));
    connect(m_configwid,SIGNAL(callLogin()),this,SLOT(showLoginWindow()));
}

//初始化系统托盘
void MainWindow::initSystemTray()
{
    /////////////////////////////////////////////////////////////////////////////////////////////////系统托盘
    system_tray = new QSystemTrayIcon();
    traymenu=new QMenu();
    system_tray->setContextMenu(traymenu);
    system_tray ->setToolTip(QString("敏求视频行为分析专家"));
    system_tray ->setIcon(QIcon(":/image/LOGO1.png"));
    system_tray->show();
    connect(system_tray , SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slot_iconIsActived(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::slot_hideanimationfinished()
{
    maskmainwid->hide();
}

void MainWindow::slot_showanimationfinished()
{
    maskmainwid->show();
}



void MainWindow::changed_videowid()
{
    m_stackwid->setCurrentIndex(2);
    videoButton->setStyleSheet("#videobutton{border-image: url(:/image/button5.png);}"
                               "#videobutton:hover{border-image: url(:/image/button.png);}"
                               "#videobutton:pressed{border-image: url(:/image/button4.png);}");
    configButton->setStyleSheet("#configbutton{border-image: url(:/image/button4.png);}"
                                "#configbutton:hover{border-image: url(:/image/button.png);}"
                                "#configbutton:pressed{border-image: url(:/image/button5.png);}");
    logButton->setStyleSheet("#configbutton{border-image: url(:/image/button4.png);}"
                                "#configbutton:hover{border-image: url(:/image/button.png);}"
                                "#configbutton:pressed{border-image: url(:/image/button5.png);}");
}


void MainWindow::changed_configwid()
{
    m_stackwid->setCurrentIndex(1);
    videoButton->setStyleSheet("#videobutton{border-image: url(:/image/button4.png);}"
                               "#videobutton:hover{border-image: url(:/image/button.png);}"
                               "#videobutton:pressed{border-image: url(:/image/button5.png);}");
    configButton->setStyleSheet("#configbutton{border-image: url(:/image/button5.png);}"
                                "#configbutton:hover{border-image: url(:/image/button.png);}"
                                "#configbutton:pressed{border-image: url(:/image/button4.png);}");
    logButton->setStyleSheet("#configbutton{border-image: url(:/image/button4.png);}"
                                "#configbutton:hover{border-image: url(:/image/button.png);}"
                                "#configbutton:pressed{border-image: url(:/image/button5.png);}");
}

void MainWindow::changed_logwid()
{
    m_stackwid->setCurrentIndex(0);
    videoButton->setStyleSheet("#videobutton{border-image: url(:/image/button4.png);}"
                               "#videobutton:hover{border-image: url(:/image/button.png);}"
                               "#videobutton:pressed{border-image: url(:/image/button5.png);}");
    configButton->setStyleSheet("#configbutton{border-image: url(:/image/button4.png);}"
                                "#configbutton:hover{border-image: url(:/image/button.png);}"
                                "#configbutton:pressed{border-image: url(:/image/button5.png);}");
    logButton->setStyleSheet("#configbutton{border-image: url(:/image/button5.png);}"
                                "#configbutton:hover{border-image: url(:/image/button.png);}"
                                "#configbutton:pressed{border-image: url(:/image/button4.png);}");
}

void MainWindow::slot_hideanimation()//隐藏登录界面的动画
{
    islogin=maskmainwid->loginupwd->isLogin();
    if(islogin)
    {
    m_loginupwincloseani->start();
    m_logindownwincloseani->start();
    m_configwid->loadCameraTree();
    }
}

void MainWindow::autoLogin()
{
    QString retCode="";
    userName="admin";
    passWD="sp84597579";
    bool ret=DATA_TRANSLATE->login(retCode);
    if(ret)
    {
        m_loginupwincloseani->start();
        m_logindownwincloseani->start();
        m_configwid->loadCameraTree();
        islogin=true;

    }
    else
    {
         islogin=false;
             maskmainwid->loginupwd->labMessage->setText("服务器连接异常,正在重新登录");
         return ;
    }
}


void MainWindow::showLoginWindow()
{
    QMessageBox::about(NULL, "登陆超时", "登陆超时，请您重新登陆");
    m_loginupwinshowani->start();
    m_logindownwinshowani->start();
}

void MainWindow::slot_iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
         {
         //点击托盘显示窗口
    case QSystemTrayIcon::Context:
    {
        traymenu->setGeometry(QCursor::pos().rx()-traymenu->width(),QCursor::pos().ry()-traymenu->height(),traymenu->width(),traymenu->height());//托盘菜单的位置
        break;
    }
         case QSystemTrayIcon::Trigger:
         case QSystemTrayIcon::DoubleClick:
         {
            showNormal();
            activateWindow();
            break;
         }
         default:
          break;
         }

}
MainWindow::~MainWindow()
{
    delete traymenu;
    delete system_tray;
    traymenu=NULL;
    system_tray=NULL;
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_MousePressed)
        {
            this->move(m_WindowPos + (event->globalPos() - m_MousePos));
        }

}
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        this->m_MousePressed = false;
    }
}


void MainWindow::closeApp()
{
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("退出程序"),
                                   QString(tr("确认退出程序?")),
                                   QMessageBox::Yes | QMessageBox::No);
    if (button == QMessageBox::No) {

    }
    else if (button == QMessageBox::Yes) {
        QString cmd=QString("killall ffmpeg");
        system(cmd.toStdString().c_str());
        cmd=QString("killall MQVIDEO");
        //system(cmd.toStdString().c_str());
        qApp->quit();  //接受退出信号，程序退出
    }
}
