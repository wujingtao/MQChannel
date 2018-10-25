#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMoveEvent>
#include <QPainter>
#include <qmath.h>
#include <QSystemTrayIcon>
#include <QMenu>
#include "loginmaskwindow.h"
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStackedWidget>
#include "qmyglobal.h"
#include "configwidget.h"
#include "logwidget.h"
#include "videowidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);


    bool    m_MousePressed;
    QPoint  m_MousePos;
    QPoint  m_WindowPos;


private:
    QMenu *traymenu;              //托盘菜单
    QSystemTrayIcon *system_tray; //系统托盘
    loginmaskwindow *maskmainwid;     //登录界面

    QStackedWidget *m_stackwid;
    ConfigWidget *m_configwid;  //配置界面
    logwidget * m_logwid;       //日志界面
    VideoWidget *m_videowid;

    QLabel * companyLogo;


    QRect location;

    QPushButton * videoButton;    //视频浏览界面按钮
    QPushButton * configButton;   //视频配置界面按钮
    QPushButton * logButton;      //日志界面
    main3Button * closeButton;    //关闭按钮
    QPushButton * maxButton;      //最大化按钮
    main3Button * minButton;      //最小化按钮

    QPropertyAnimation *m_loginupwincloseani;
    QPropertyAnimation *m_loginupwinshowani;

    QPropertyAnimation *m_logindownwincloseani;
    QPropertyAnimation *m_logindownwinshowani;

     QTimer  *timer_show;

    void initWindow(); //初始化窗口
    void initLoginWindow();//初始化登录窗口
    void initSystemTray(); //初始化托盘
    void autoLogin();

public slots:
    void showLoginWindow();

private slots:
    void slot_iconIsActived(QSystemTrayIcon::ActivationReason);
    void slot_hideanimationfinished();
    void slot_showanimationfinished();
    void slot_hideanimation();
    void changed_videowid();
    void changed_configwid();
    void changed_logwid();
    void closeApp();
};

#endif // MAINWINDOW_H
